/*
 * mcache.c
 *
 * Copyright (c) 2010,2011, Dan Magenheimer, Oracle Corp.
 * Copyright (c) 2010,2011, Nitin Gupta
 *
 * Zcache provides an in-kernel "host implementation" for transcendent memory
 * and, thus indirectly, for cleancache and frontswap.  Zcache includes two
 * page-accessible memory [1] interfaces, both utilizing the crypto compression
 * API:
 * 1) "compression buddies" ("zbud") is used for ephemeral pages
 * 2) zsmalloc is used for persistent pages.
 * Xvmalloc (based on the TLSF allocator) has very low fragmentation
 * so maximizes space efficiency, while zbud allows pairs (and potentially,
 * in the future, more than a pair of) compressed pages to be closely linked
 * so that reclaiming can be done via the kernel's physical-page-oriented
 * "shrinker" interface.
 *
 * [1] For a definition of page-accessible memory (aka PAM), see:
 *   http://marc.info/?l=linux-mm&m=127811271605009
 */

/*
  mcache driver: Hooks into tmem
	page management handled by tmem (pampd ops).

*/

#include <linux/module.h>
#include <linux/cpu.h>
#include <linux/highmem.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/atomic.h>
#include <linux/math64.h>
#include <linux/crypto.h>
#include <linux/string.h>
#include <linux/bootmem.h> //ASIM
#include <linux/kthread.h> //ASIM
#include "tmem.h"

#include <linux/spinlock_types.h>
//#include "../zsmalloc/zsmalloc.h"

#define COLDMEM

#if (!defined(CONFIG_CLEANCACHE) && !defined(CONFIG_FRONTSWAP))
#error "zcache is useless without CONFIG_CLEANCACHE or CONFIG_FRONTSWAP"
#endif
#ifdef CONFIG_CLEANCACHE
#include <linux/cleancache.h>
#endif

#if 0
/* this is more aggressive but may cause other problems? */
#define ZCACHE_GFP_MASK	(GFP_ATOMIC | __GFP_NORETRY | __GFP_NOWARN)
#else
#define ZCACHE_GFP_MASK \
	(__GFP_FS | __GFP_NORETRY | __GFP_NOWARN | __GFP_NOMEMALLOC)
#endif

#define ZCACHE_COMP_NAME_SZ CRYPTO_MAX_ALG_NAME
static char zcache_comp_name[ZCACHE_COMP_NAME_SZ];

#define MAX_POOLS_PER_CLIENT 16

#define MAX_CLIENTS 16
#define LOCAL_CLIENT ((uint16_t)-1)

MODULE_LICENSE("GPL");

struct zcache_client {
	struct tmem_pool *tmem_pools[MAX_POOLS_PER_CLIENT];
	struct zs_pool *zspool;
	bool allocated;
	atomic_t refcount;
};

static struct zcache_client zcache_host;
/* List of zcache clients */
static struct zcache_client zcache_clients[MAX_CLIENTS];

static inline uint16_t get_client_id_from_client(struct zcache_client *cli)
{
	BUG_ON(cli == NULL);
	if (cli == &zcache_host)
		return LOCAL_CLIENT;
	return cli - &zcache_clients[0];
}

static inline bool is_local_client(struct zcache_client *cli)
{
	return cli == &zcache_host;
}

/* crypto API for zcache  */
#define ZCACHE_COMP_NAME_SZ CRYPTO_MAX_ALG_NAME
static char zcache_hash_name[ZCACHE_COMP_NAME_SZ];
static struct crypto_comp * __percpu *zcache_comp_pcpu_tfms;

enum comp_op {
	ZCACHE_COMPOP_CHECKSUM,
	ZCACHE_COMPOP_DCHECKSUM
};

static inline int zcache_comp_op(enum comp_op op,
				const u8 *src, unsigned int slen,
				u8 *dst, unsigned int *dlen)
{
	struct crypto_comp *tfm;
	int ret;

	BUG_ON(!zcache_comp_pcpu_tfms);
	tfm = *per_cpu_ptr(zcache_comp_pcpu_tfms, get_cpu());
	BUG_ON(!tfm);
	switch (op) {
	case ZCACHE_COMPOP_CHECKSUM:
		ret = crypto_comp_compress(tfm, src, slen, dst, dlen);
		break;
	case ZCACHE_COMPOP_DCHECKSUM:
		ret = crypto_comp_decompress(tfm, src, slen, dst, dlen);
		break;
	}
	put_cpu();
	return ret;
}

/**********
 * Compression buddies ("zbud") provides for packing two (or, possibly
 * in the future, more) compressed ephemeral pages into a single "raw"
 * (physical) page and tracking them with data structures so that
 * the raw pages can be easily reclaimed.
 *
 * A zbud page ("zbpg") is an aligned page containing a list_head,
 * a lock, and two "zbud headers".  The remainder of the physical
 * page is divided up into aligned 64-byte "chunks" which contain
 * the compressed data for zero, one, or two zbuds.  Each zbpg
 * resides on: (1) an "unused list" if it has no zbuds; (2) a
 * "buddied" list if it is fully populated  with two zbuds; or
 * (3) one of PAGE_SIZE/64 "unbuddied" lists indexed by how many chunks
 * the one unbuddied zbud uses.  The data inside a zbpg cannot be
 * read or written unless the zbpg's lock is held.
 */

#define MPAGE_SENTINEL  0x43214321

struct mpage {
	uint16_t client_id;
	uint16_t pool_id;
	struct tmem_oid oid;
	uint32_t index;
	uint32_t cksum; //ASIM
	void *page;
	spinlock_t lock;
	struct list_head mpage_list;
	DECL_SENTINEL
};


static LIST_HEAD(zbpg_all_list);
static unsigned long zcache_zbpg_all_list_count;

/* protects the unused page list */
static DEFINE_SPINLOCK(zbpg_all_list_spinlock);

static atomic_t zcache_zbud_curr_raw_pages;
static atomic_t zcache_zbud_curr_zpages;
static unsigned long zcache_zbud_curr_zbytes;
static unsigned long zcache_zbud_cumul_zpages;
static unsigned long zcache_zbud_cumul_zbytes;

/* forward references */
static void *zcache_get_free_page(void);
static void zcache_free_page(void *p);

/*
 * zbud helper functions
 */

static struct mpage *zbud_create(uint16_t client_id, uint16_t pool_id,
					struct tmem_oid *oid,
					uint32_t index, struct page *page,
					void *cdata, unsigned size)
{
	struct mpage *zbpg = NULL;
	char *to;

	zbpg = kmalloc(sizeof(struct mpage), GFP_ATOMIC);
	spin_lock_init(&zbpg->lock);
	INIT_LIST_HEAD(&zbpg->mpage_list);

	spin_lock(&zbpg->lock);
	zbpg->page = zcache_get_free_page();
	//zbpg->page = __get_free_page(ZCACHE_GFP_MASK);
	if (unlikely(zbpg == NULL))
		goto out;
	/* ok, have a page, now compress the data before taking locks */

	SET_SENTINEL(zbpg, MPAGE);
	//zh->size = size;
	zbpg->index = index;
	zbpg->oid = *oid;
	zbpg->pool_id = pool_id;
	zbpg->client_id = client_id;
	
	memcpy(to, cdata, size);

	spin_lock(&zbpg_all_list_spinlock);

 	list_add(&zbpg->mpage_list, &zbpg_all_list);
	spin_unlock(&zbpg_all_list_spinlock);

	spin_unlock(&zbpg->lock);

	atomic_inc(&zcache_zbud_curr_zpages);
	zcache_zbud_cumul_zpages++;
	zcache_zbud_curr_zbytes += size;
	zcache_zbud_cumul_zbytes += size;
	printk ("Done puttng a page in mcache.\n"); 
out:
	return zbpg;
}


/* The job here is too return checksum/compare and return */
static int zbud_decompress(struct page *page, struct mpage *zh)
{
	unsigned int out_len = PAGE_SIZE;
	char *to_va, *from_va;
	unsigned size;
	int ret = 0;

	spin_lock(&zh->lock);
	ASSERT_SENTINEL(zh, MPAGE);
	to_va = kmap_atomic(page);
	//size = zh->size;
	from_va = zh->page; 
	ret = zcache_comp_op(ZCACHE_COMPOP_DCHECKSUM, from_va, size,
				to_va, &out_len);
	BUG_ON(ret);
	BUG_ON(out_len != PAGE_SIZE);
	kunmap_atomic(to_va);
	spin_unlock(&zh->lock);
	return ret;
}

/*
 * The following routines handle shrinking of ephemeral pages by evicting
 * pages "least valuable" first.
 */

static unsigned long zcache_evicted_raw_pages;

static struct tmem_pool *zcache_get_pool_by_id(uint16_t cli_id,
						uint16_t poolid);
static void zcache_put_pool(struct tmem_pool *pool);

static void zbud_free_raw_page(struct mpage *zbpg)
{
	ASSERT_SENTINEL(zbpg, MPAGE);
	ASSERT_SPINLOCK(&zbpg->lock);
	spin_unlock(&zbpg->lock);
	atomic_dec(&zcache_zbud_curr_raw_pages);
	zcache_free_page(zbpg->page);
	kfree(zbpg);
}


/*
 * Flush and free all zbuds in a zbpg, then free the pageframe
 */

static void zbud_free_and_delist(struct mpage *zbpg)
{
	spin_lock(&zbpg->lock);
	zbud_free_raw_page(zbpg);
	return;
}


 // This is changed now and one zbpg has only one page 
static void zbud_evict_zbpg(struct mpage *zbpg)
{
	uint32_t pool_id, client_id;
	uint32_t index;
	struct tmem_oid oid;
	struct tmem_pool *pool;

	pool_id = zbpg->pool_id;
	client_id = zbpg->client_id;
	oid = zbpg->oid;
	index = zbpg->index;
	ASSERT_SPINLOCK(&zbpg->lock);
	
	//zbud_free(zh);
	//TODO free zh

	spin_unlock(&zbpg->lock);
	pool = zcache_get_pool_by_id(client_id, pool_id);
	if (pool != NULL) {
		tmem_flush_page(pool, &oid, index);
		zcache_put_pool(pool);
	}

	ASSERT_SENTINEL(zbpg, MPAGE);
	spin_lock(&zbpg->lock);
	zbud_free_raw_page(zbpg);
	zcache_evicted_raw_pages++;
}

/*
 * Free nr pages.  This code is funky because we want to hold the locks
 * protecting various lists for as short a time as possible, and in some
 * circumstances the list may change asynchronously when the list lock is
 * not held.  In some cases we also trylock not only to avoid waiting on a
 * page in use by another cpu, but also to avoid potential deadlock due to
 * lock inversion.
 */
static void zbud_evict_pages(int nr)
{
	struct mpage *zbpg = NULL;
	

	spin_lock_bh(&zbpg_all_list_spinlock);
retry_all_list:
	if (!list_empty(&zbpg_all_list)) {
		/* can't walk list here, since it may change when unlocked */
		zbpg = list_first_entry(&zbpg_all_list, struct mpage, mpage_list);
		zcache_zbpg_all_list_count--;
		list_del_init(&zbpg_all_list);
		atomic_dec(&zcache_zbud_curr_raw_pages);
		zbud_evict_zbpg(zbpg);
		zcache_evicted_raw_pages++;
		if (--nr <= 0)
			goto out;
		goto retry_all_list;
	}
out:	
	spin_unlock_bh(&zbpg_all_list_spinlock);


	// Lock the list

	//Scan the zpbg list

	//free page

	//evict page

	//increment statistic

	//local_bh_enable();
	return;
}



/*
 * byte count defining poor *mean* compression; pages with greater zsize
 * will be rejected until sufficient better-compressed pages are accepted
 * driving the mean below this threshold
 */


/* Decompress ==  compute checksum post access. 

static void zv_decompress(struct page *page, void *handle)
{
	unsigned int clen = PAGE_SIZE;
	char *to_va;
	int ret;
	struct zv_hdr *zv;

	zv = zs_map_object(zcache_host.zspool, handle);
	BUG_ON(zv->size == 0);
	ASSERT_SENTINEL(zv, ZVH);
	to_va = kmap_atomic(page);
	ret = zcache_comp_op(ZCACHE_COMPOP_DECOMPRESS, (char *)zv + sizeof(*zv),
				zv->size, to_va, &clen);
	kunmap_atomic(to_va);
	zs_unmap_object(zcache_host.zspool, handle);
	BUG_ON(ret);
	BUG_ON(clen != PAGE_SIZE);
}
*/


/* useful stats not collected by cleancache or frontswap */
static unsigned long zcache_flush_total;
static unsigned long zcache_flush_found;
static unsigned long zcache_flobj_total;
static unsigned long zcache_flobj_found;
static unsigned long zcache_failed_eph_puts;

/*
 * Tmem operations assume the poolid implies the invoking client.
 * Zcache only has one client (the kernel itself): LOCAL_CLIENT.
 * RAMster has each client numbered by cluster node, and a KVM version
 * of zcache would have one client per guest and each client might
 * have a poolid==N.
 */
static struct tmem_pool *zcache_get_pool_by_id(uint16_t cli_id, uint16_t poolid)
{
	struct tmem_pool *pool = NULL;
	struct zcache_client *cli = NULL;

	if (cli_id == LOCAL_CLIENT)
		cli = &zcache_host;
	else {
		if (cli_id >= MAX_CLIENTS)
			goto out;
		cli = &zcache_clients[cli_id];
		if (cli == NULL)
			goto out;
		atomic_inc(&cli->refcount);
	}
	if (poolid < MAX_POOLS_PER_CLIENT) {
		pool = cli->tmem_pools[poolid];
		if (pool != NULL)
			atomic_inc(&pool->refcount);
	}
out:
	return pool;
}

static void zcache_put_pool(struct tmem_pool *pool)
{
	struct zcache_client *cli = NULL;

	if (pool == NULL)
		BUG();
	cli = pool->client;
	atomic_dec(&pool->refcount);
	atomic_dec(&cli->refcount);
}

int zcache_new_client(uint16_t cli_id)
{
	struct zcache_client *cli = NULL;
	int ret = -1;

	if (cli_id == LOCAL_CLIENT)
		cli = &zcache_host;
	else if ((unsigned int)cli_id < MAX_CLIENTS)
		cli = &zcache_clients[cli_id];
	if (cli == NULL)
		goto out;
	if (cli->allocated)
		goto out;
	cli->allocated = 1;
	ret = 0;
	printk ("zcache_new_client: zcache registered.\n");
out:
	return ret;
}

/* counters for debugging */
static unsigned long zcache_failed_get_free_pages;
static unsigned long zcache_failed_alloc;
static unsigned long zcache_put_to_flush;

/*
 * for now, used named slabs so can easily track usage; later can
 * either just use kmalloc, or perhaps add a slab-like allocator
 * to more carefully manage total memory utilization
 */
static struct kmem_cache *zcache_objnode_cache;
static struct kmem_cache *zcache_obj_cache;
static atomic_t zcache_curr_obj_count = ATOMIC_INIT(0);
static unsigned long zcache_curr_obj_count_max;
static atomic_t zcache_curr_objnode_count = ATOMIC_INIT(0);
static unsigned long zcache_curr_objnode_count_max;

/*
 * to avoid memory allocation recursion (e.g. due to direct reclaim), we
 * preload all necessary data structures so the hostops callbacks never
 * actually do a malloc
 */
struct zcache_preload {
	void *page;
	struct tmem_obj *obj;
	int nr;
	struct tmem_objnode *objnodes[OBJNODE_TREE_MAX_PATH];
};
static DEFINE_PER_CPU(struct zcache_preload, zcache_preloads) = { 0, };

#define COLDMEM_MAP_MAX_PAGES 16384 
static char cold_pages_map[COLDMEM_MAP_MAX_PAGES];
static char cold_pages_kmap[COLDMEM_MAP_MAX_PAGES];
static spinlock_t cold_pages_lock;
static void *coldmem_pages;
static int zcache_map_pages(void);

static void * zcache_get_coldmempage(void)
{
		
	int i = 0;

	spin_lock(&cold_pages_lock);	
	while (cold_pages_map[i] != 0)	{
		i++;
	}
	
	if (i > COLDMEM_MAP_MAX_PAGES)	{
		printk ("zcache: memory allocation failed.\n");
		return NULL;
	}	
	cold_pages_map[i] = 1;
	spin_unlock(&cold_pages_lock);

	//kthread_run(zcache_map_pages, NULL, "mcache-kmapper");
	//printk ("Allocated page for request %d.\n", i);
	
	/* THe problem we have here is that kmap can sleep.
	 * We need to somehow keep this memory mapped at all
	 * times.
	*/
	//return kmap((void *) (i*1024*4 +(char *)coldmem_pages));		
	return (void *) (i*1024*4 +(char *)coldmem_pages);

}

static int zcache_reserve_coldmemory(void)
{
	void * pages = (void *)alloc_pages (GFP_HIGHUSER, 14);  
	coldmem_pages = kmap_atomic(pages);

	//kthread_run(zcache_map_pages, NULL, "mcache-kmapper");	

 	/*	
	if (coldmem_pages == NULL)	{
		printk ("Memory allocation failed.\n");
	}
	*/	
	return 0;
}

static void zcache_put_coldmempage(void * addr)
{
	int index = ((char *) addr - (char *) coldmem_pages)/1024;

	if ((index <0) || (index > COLDMEM_MAP_MAX_PAGES))
		return;

	spin_lock(&cold_pages_lock);
	cold_pages_map[index] = 0;
	spin_unlock(&cold_pages_lock);		
	return ;
}



static int zcache_do_preload(struct tmem_pool *pool)
{
	struct zcache_preload *kp;
	struct tmem_objnode *objnode;
	struct tmem_obj *obj;
	void *page;
	int ret = -ENOMEM;

	if (unlikely(zcache_objnode_cache == NULL))
		goto out;
	if (unlikely(zcache_obj_cache == NULL))
		goto out;
	preempt_disable();
	kp = &__get_cpu_var(zcache_preloads);
	while (kp->nr < ARRAY_SIZE(kp->objnodes)) {
		preempt_enable_no_resched();
		objnode = kmem_cache_alloc(zcache_objnode_cache,
				ZCACHE_GFP_MASK);
		if (unlikely(objnode == NULL)) {
			zcache_failed_alloc++;
			goto out;
		}
		preempt_disable();
		kp = &__get_cpu_var(zcache_preloads);
		if (kp->nr < ARRAY_SIZE(kp->objnodes))
			kp->objnodes[kp->nr++] = objnode;
		else
			kmem_cache_free(zcache_objnode_cache, objnode);
	}
	preempt_enable_no_resched();
	obj = kmem_cache_alloc(zcache_obj_cache, ZCACHE_GFP_MASK);
	if (unlikely(obj == NULL)) {
		zcache_failed_alloc++;
		goto out;
	}
	/* ASIM: These are the pages that you want to be allocated
	* from the cold memory pool.
	*/
	//page = (void *)__get_free_page(ZCACHE_GFP_MASK);
	page = (void *)zcache_get_coldmempage(); //ASIM


	if (page == NULL)	{
		panic ("zcache alloctor failed.\n");
		//page = (void *)__get_free_page(ZCACHE_GFP_MASK);
	}


	if (unlikely(page == NULL)) {
		zcache_failed_get_free_pages++;
		kmem_cache_free(zcache_obj_cache, obj);
		goto out;
	}
	preempt_disable();
	kp = &__get_cpu_var(zcache_preloads);
	if (kp->obj == NULL)
		kp->obj = obj;
	else
		kmem_cache_free(zcache_obj_cache, obj);
	if (kp->page == NULL)
		kp->page = page;
	else
		{
			printk ("preload failed.\n");
			free_page((unsigned long)page);
		}	
	ret = 0;
out:
	return ret;
}

static void *zcache_get_free_page(void)
{

	void *page;
	page = (void *)zcache_get_coldmempage();
	BUG_ON(page == NULL);
	return page;
}

static void zcache_free_page(void *p)
{
	//free_page((unsigned long)p);
	zcache_put_coldmempage(p);
}

/*
 * zcache implementation for tmem host ops
 */

static struct tmem_objnode *zcache_objnode_alloc(struct tmem_pool *pool)
{
	struct tmem_objnode *objnode = NULL;
	unsigned long count;
	struct zcache_preload *kp;

	kp = &__get_cpu_var(zcache_preloads);
	if (kp->nr <= 0)
		goto out;
	objnode = kp->objnodes[kp->nr - 1];
	BUG_ON(objnode == NULL);
	kp->objnodes[kp->nr - 1] = NULL;
	kp->nr--;
	count = atomic_inc_return(&zcache_curr_objnode_count);
	if (count > zcache_curr_objnode_count_max)
		zcache_curr_objnode_count_max = count;
out:
	return objnode;
}

static void zcache_objnode_free(struct tmem_objnode *objnode,
					struct tmem_pool *pool)
{
	atomic_dec(&zcache_curr_objnode_count);
	BUG_ON(atomic_read(&zcache_curr_objnode_count) < 0);
	kmem_cache_free(zcache_objnode_cache, objnode);
}

static struct tmem_obj *zcache_obj_alloc(struct tmem_pool *pool)
{
	struct tmem_obj *obj = NULL;
	unsigned long count;
	struct zcache_preload *kp;

	kp = &__get_cpu_var(zcache_preloads);
	obj = kp->obj;
	BUG_ON(obj == NULL);
	kp->obj = NULL;
	count = atomic_inc_return(&zcache_curr_obj_count);
	if (count > zcache_curr_obj_count_max)
		zcache_curr_obj_count_max = count;
	return obj;
}

static void zcache_obj_free(struct tmem_obj *obj, struct tmem_pool *pool)
{
	atomic_dec(&zcache_curr_obj_count);
	BUG_ON(atomic_read(&zcache_curr_obj_count) < 0);
	kmem_cache_free(zcache_obj_cache, obj);
}

static struct tmem_hostops zcache_hostops = {
	.obj_alloc = zcache_obj_alloc,
	.obj_free = zcache_obj_free,
	.objnode_alloc = zcache_objnode_alloc,
	.objnode_free = zcache_objnode_free,
};

/*
 * zcache implementations for PAM page descriptor ops
 */

static atomic_t zcache_curr_eph_pampd_count = ATOMIC_INIT(0);
static unsigned long zcache_curr_eph_pampd_count_max;
static atomic_t zcache_curr_pers_pampd_count = ATOMIC_INIT(0);
static unsigned long zcache_curr_pers_pampd_count_max;

/* forward reference */
static int zcache_compress(struct page *from, void **out_va, unsigned *out_len);

static void *zcache_pampd_create(char *data, size_t size, bool raw, int eph,
				struct tmem_pool *pool, struct tmem_oid *oid,
				 uint32_t index)
{
	void *pampd = NULL, *cdata;
	unsigned clen;
	unsigned long count;
	struct page *page = (struct page *)(data);
	struct zcache_client *cli = pool->client;
	uint16_t client_id = get_client_id_from_client(cli);

	//ASSERT(page != NULL);
	/* mcache is always ephemeral */
	if (eph) {
		//ASIM calculate checksum and add to hdr
		cdata = page;
		clen = PAGE_SIZE;
		pampd = (void *)zbud_create(client_id, pool->pool_id, oid,
						index, page, cdata, clen);
		if (pampd != NULL) {
			count = atomic_inc_return(&zcache_curr_eph_pampd_count);
			if (count > zcache_curr_eph_pampd_count_max)
				zcache_curr_eph_pampd_count_max = count;
		}
	} else {
		printk ("BUGBUG..\n");
	}
	return pampd;
}

/*
 * fill the pageframe corresponding to the struct page with the data
 * from the passed pampd
 */
static int zcache_pampd_get_data(char *data, size_t *bufsize, bool raw,
					void *pampd, struct tmem_pool *pool,
					struct tmem_oid *oid, uint32_t index)
{
	int ret = 0;

	BUG_ON(is_ephemeral(pool));

	/* Return page, check checksum and return */
	//zv_decompress((struct page *)(data), pampd);
			
	return ret;
}

/*
 * fill the pageframe corresponding to the struct page with the data
 * from the passed pampd
 */
static int zcache_pampd_get_data_and_free(char *data, size_t *bufsize, bool raw,
					void *pampd, struct tmem_pool *pool,
					struct tmem_oid *oid, uint32_t index)
{
	int ret = 0;

	BUG_ON(!is_ephemeral(pool));
	//zbud_decompress((struct page *)(data), pampd);
	zbud_free_and_delist((struct mpage *)pampd);
	atomic_dec(&zcache_curr_eph_pampd_count);
	return ret;
}

/*
 * free the pampd and remove it from any zcache lists
 * pampd must no longer be pointed to from any tmem data structures!
 */
static void zcache_pampd_free(void *pampd, struct tmem_pool *pool,
				struct tmem_oid *oid, uint32_t index)
{

	if (is_ephemeral(pool)) {
		zbud_free_and_delist((struct mpage *)pampd);
		atomic_dec(&zcache_curr_eph_pampd_count);
		BUG_ON(atomic_read(&zcache_curr_eph_pampd_count) < 0);
	} else {
		printk ("BUGBUG. persistent pool called.\n");
	}
}

static void zcache_pampd_free_obj(struct tmem_pool *pool, struct tmem_obj *obj)
{
}

static void zcache_pampd_new_obj(struct tmem_obj *obj)
{
}

static int zcache_pampd_replace_in_obj(void *pampd, struct tmem_obj *obj)
{
	return -1;
}

static bool zcache_pampd_is_remote(void *pampd)
{
	return 0;
}

static struct tmem_pamops zcache_pamops = {
	.create = zcache_pampd_create,
	.get_data = zcache_pampd_get_data,
	.get_data_and_free = zcache_pampd_get_data_and_free,
	.free = zcache_pampd_free,
	.free_obj = zcache_pampd_free_obj,
	.new_obj = zcache_pampd_new_obj,
	.replace_in_obj = zcache_pampd_replace_in_obj,
	.is_remote = zcache_pampd_is_remote,
};

/*
 * zcache compression/decompression and related per-cpu stuff
 */

static DEFINE_PER_CPU(unsigned char *, zcache_dstmem);
#define ZCACHE_DSTMEM_ORDER 1

/*
static int zcache_compress(struct page *from, void **out_va, unsigned *out_len)
{
	int ret = 0;
	unsigned char *dmem = __get_cpu_var(zcache_dstmem);
	char *from_va;

	//BUG_ON(!irqs_disabled());
	if (unlikely(dmem == NULL))
		goto out;  // no buffer or no compressor so can't compress 
	*out_len = PAGE_SIZE << ZCACHE_DSTMEM_ORDER;
	from_va = kmap_atomic(from);
	mb();
	ret = zcache_comp_op(ZCACHE_COMPOP_CHECKSUM, from_va, PAGE_SIZE, dmem,
				out_len);
	BUG_ON(ret);
	*out_va = dmem;
	kunmap_atomic(from_va);
	ret = 1;
out:
	return ret;
}
*/

static int zcache_comp_cpu_up(int cpu)
{
	struct crypto_comp *tfm;

	tfm = crypto_alloc_hash(zcache_comp_name, 0, 0);
	if (IS_ERR(tfm))
		return NOTIFY_BAD;
	*per_cpu_ptr(zcache_comp_pcpu_tfms, cpu) = tfm;
	return NOTIFY_OK;
}

static void zcache_comp_cpu_down(int cpu)
{
	struct crypto_comp *tfm;

	tfm = *per_cpu_ptr(zcache_comp_pcpu_tfms, cpu);
	crypto_free_comp(tfm);
	*per_cpu_ptr(zcache_comp_pcpu_tfms, cpu) = NULL;
}

static int zcache_cpu_notifier(struct notifier_block *nb,
				unsigned long action, void *pcpu)
{
	int ret, cpu = (long)pcpu;
	struct zcache_preload *kp;

	switch (action) {
	case CPU_UP_PREPARE:
		ret = zcache_comp_cpu_up(cpu);
		if (ret != NOTIFY_OK) {
			pr_err("zcache: can't allocate compressor transform\n");
			return ret;
		}
		per_cpu(zcache_dstmem, cpu) = (void *)__get_free_pages(
			GFP_KERNEL | __GFP_REPEAT, ZCACHE_DSTMEM_ORDER);
		printk ("zcache allocated pages.\n");
		break;
	case CPU_DEAD:
	case CPU_UP_CANCELED:
		zcache_comp_cpu_down(cpu);
		free_pages((unsigned long)per_cpu(zcache_dstmem, cpu),
			ZCACHE_DSTMEM_ORDER);
		printk ("warn:zcache freed pages.\n");
		per_cpu(zcache_dstmem, cpu) = NULL;
		kp = &per_cpu(zcache_preloads, cpu);
		while (kp->nr) {
			kmem_cache_free(zcache_objnode_cache,
					kp->objnodes[kp->nr - 1]);
			kp->objnodes[kp->nr - 1] = NULL;
			kp->nr--;
		}
		if (kp->obj) {
			kmem_cache_free(zcache_obj_cache, kp->obj);
			kp->obj = NULL;
		}
		if (kp->page) {
			free_page((unsigned long)kp->page);
			kp->page = NULL;
		}
		break;
	default:
		break;
	}
	return NOTIFY_OK;
}

static struct notifier_block zcache_cpu_notifier_block = {
	.notifier_call = zcache_cpu_notifier
};

#ifdef CONFIG_SYSFS
#define ZCACHE_SYSFS_RO(_name) \
	static ssize_t zcache_##_name##_show(struct kobject *kobj, \
				struct kobj_attribute *attr, char *buf) \
	{ \
		return sprintf(buf, "%lu\n", zcache_##_name); \
	} \
	static struct kobj_attribute zcache_##_name##_attr = { \
		.attr = { .name = __stringify(_name), .mode = 0444 }, \
		.show = zcache_##_name##_show, \
	}

#define ZCACHE_SYSFS_RO_ATOMIC(_name) \
	static ssize_t zcache_##_name##_show(struct kobject *kobj, \
				struct kobj_attribute *attr, char *buf) \
	{ \
	    return sprintf(buf, "%d\n", atomic_read(&zcache_##_name)); \
	} \
	static struct kobj_attribute zcache_##_name##_attr = { \
		.attr = { .name = __stringify(_name), .mode = 0444 }, \
		.show = zcache_##_name##_show, \
	}

#define ZCACHE_SYSFS_RO_CUSTOM(_name, _func) \
	static ssize_t zcache_##_name##_show(struct kobject *kobj, \
				struct kobj_attribute *attr, char *buf) \
	{ \
	    return _func(buf); \
	} \
	static struct kobj_attribute zcache_##_name##_attr = { \
		.attr = { .name = __stringify(_name), .mode = 0444 }, \
		.show = zcache_##_name##_show, \
	}

ZCACHE_SYSFS_RO(curr_obj_count_max);
ZCACHE_SYSFS_RO(curr_objnode_count_max);
ZCACHE_SYSFS_RO(flush_total);
ZCACHE_SYSFS_RO(flush_found);
ZCACHE_SYSFS_RO(flobj_total);
ZCACHE_SYSFS_RO(flobj_found);
ZCACHE_SYSFS_RO(failed_eph_puts);
ZCACHE_SYSFS_RO(zbud_curr_zbytes);
ZCACHE_SYSFS_RO(zbud_cumul_zpages);
ZCACHE_SYSFS_RO(zbud_cumul_zbytes);
ZCACHE_SYSFS_RO(evicted_raw_pages);
ZCACHE_SYSFS_RO(failed_get_free_pages);
ZCACHE_SYSFS_RO(failed_alloc);
ZCACHE_SYSFS_RO(put_to_flush);
ZCACHE_SYSFS_RO_ATOMIC(zbud_curr_raw_pages);
ZCACHE_SYSFS_RO_ATOMIC(zbud_curr_zpages);
ZCACHE_SYSFS_RO_ATOMIC(curr_obj_count);
ZCACHE_SYSFS_RO_ATOMIC(curr_objnode_count);

static struct attribute *zcache_attrs[] = {
	&zcache_curr_obj_count_attr.attr,
	&zcache_curr_obj_count_max_attr.attr,
	&zcache_curr_objnode_count_attr.attr,
	&zcache_curr_objnode_count_max_attr.attr,
	&zcache_flush_total_attr.attr,
	&zcache_flobj_total_attr.attr,
	&zcache_flush_found_attr.attr,
	&zcache_flobj_found_attr.attr,
	&zcache_failed_eph_puts_attr.attr,
	&zcache_zbud_curr_raw_pages_attr.attr,
	&zcache_zbud_curr_zpages_attr.attr,
	&zcache_zbud_curr_zbytes_attr.attr,
	&zcache_zbud_cumul_zpages_attr.attr,
	&zcache_zbud_cumul_zbytes_attr.attr,
	&zcache_evicted_raw_pages_attr.attr,
	&zcache_failed_get_free_pages_attr.attr,
	&zcache_failed_alloc_attr.attr,
	&zcache_put_to_flush_attr.attr,
	NULL,
};

static struct attribute_group zcache_attr_group = {
	.attrs = zcache_attrs,
	.name = "zcache",
};

#endif /* CONFIG_SYSFS */
/*
 * When zcache is disabled ("frozen"), pools can be created and destroyed,
 * but all puts (and thus all other operations that require memory allocation)
 * must fail.  If zcache is unfrozen, accepts puts, then frozen again,
 * data consistency requires all puts while frozen to be converted into
 * flushes.
 */
static bool zcache_freeze;

/*
 * zcache shrinker interface (only useful for ephemeral pages, so zbud only)
 */
static int shrink_zcache_memory(struct shrinker *shrink,
				struct shrink_control *sc)
{
	int ret = -1;
	int nr = sc->nr_to_scan;
	gfp_t gfp_mask = sc->gfp_mask;

	if (nr >= 0) {
		if (!(gfp_mask & __GFP_FS))
			/* does this case really need to be skipped? */
			goto out;
		zbud_evict_pages(nr);
	}
	ret = (int)atomic_read(&zcache_zbud_curr_raw_pages);
out:
	return ret;
}

static struct shrinker zcache_shrinker = {
	.shrink = shrink_zcache_memory,
	.seeks = DEFAULT_SEEKS,
};

/*
 * zcache shims between cleancache/frontswap ops and tmem
 * mcache_put_page, computes a checksum and places a page in cold memory/
 */

static int mcache_put_page(int cli_id, int pool_id, struct tmem_oid *oidp,
				uint32_t index, struct page *page)
{
	struct tmem_pool *pool;
	int ret = -1;

	printk ("putting page %p in zcache.\n", page);
	// FIX THIS
	//BUG_ON(!irqs_disabled()); //why?
	pool = zcache_get_pool_by_id(cli_id, pool_id);
	if (unlikely(pool == NULL))
		goto out;
//	if (!zcache_freeze && zcache_do_preload(pool) == 0) {
		/* preload does preempt_disable on success */
		/* tmem_put places a page by invoking pampd callbacks. */
		ret = tmem_put(pool, oidp, index, (char *)(page),
				PAGE_SIZE, 0, is_ephemeral(pool));
		if (ret < 0) {
			if (is_ephemeral(pool))
				zcache_failed_eph_puts++;
		}
		zcache_put_pool(pool);
		preempt_enable_no_resched();
/*		
	} else {
		zcache_put_to_flush++;
		if (atomic_read(&pool->obj_count) > 0)
			// the put fails whether the flush succeeds or not 
			(void)tmem_flush_page(pool, oidp, index);
		zcache_put_pool(pool);
	}
	*/
out:
	return ret;
}

static int mcache_get_page(int cli_id, int pool_id, struct tmem_oid *oidp,
				uint32_t index, struct page *page)
{
	struct tmem_pool *pool;
	int ret = -1;
	unsigned long flags;
	size_t size = PAGE_SIZE;

	local_irq_save(flags);
	//printk ("getting page %p in zcache.\n", page);
	pool = zcache_get_pool_by_id(cli_id, pool_id);
	if (likely(pool != NULL)) {
		if (atomic_read(&pool->obj_count) > 0)
			/* tmem ops retrieve a page using mcache pampd ops */
			ret = tmem_get(pool, oidp, index, (char *)(page),
					&size, 0, is_ephemeral(pool));
		zcache_put_pool(pool);
	}
	local_irq_restore(flags);
	return ret;
}

static int zcache_flush_page(int cli_id, int pool_id,
				struct tmem_oid *oidp, uint32_t index)
{
	struct tmem_pool *pool;
	int ret = -1;
	unsigned long flags;

	local_irq_save(flags);
	zcache_flush_total++;
	pool = zcache_get_pool_by_id(cli_id, pool_id);
	if (likely(pool != NULL)) {
		if (atomic_read(&pool->obj_count) > 0)
			ret = tmem_flush_page(pool, oidp, index);
		zcache_put_pool(pool);
	}
	if (ret >= 0)
		zcache_flush_found++;
	local_irq_restore(flags);
	return ret;
}

static int zcache_flush_object(int cli_id, int pool_id,
				struct tmem_oid *oidp)
{
	struct tmem_pool *pool;
	int ret = -1;
	unsigned long flags;

	local_irq_save(flags);
	zcache_flobj_total++;
	pool = zcache_get_pool_by_id(cli_id, pool_id);
	if (likely(pool != NULL)) {
		if (atomic_read(&pool->obj_count) > 0)
			ret = tmem_flush_object(pool, oidp);
		zcache_put_pool(pool);
	}
	if (ret >= 0)
		zcache_flobj_found++;
	local_irq_restore(flags);
	return ret;
}

static int zcache_destroy_pool(int cli_id, int pool_id)
{
	struct tmem_pool *pool = NULL;
	struct zcache_client *cli = NULL;
	int ret = -1;

	if (pool_id < 0)
		goto out;
	if (cli_id == LOCAL_CLIENT)
		cli = &zcache_host;
	else if ((unsigned int)cli_id < MAX_CLIENTS)
		cli = &zcache_clients[cli_id];
	if (cli == NULL)
		goto out;
	atomic_inc(&cli->refcount);
	pool = cli->tmem_pools[pool_id];
	if (pool == NULL)
		goto out;
	cli->tmem_pools[pool_id] = NULL;
	/* wait for pool activity on other cpus to quiesce */
	while (atomic_read(&pool->refcount) != 0)
		;
	atomic_dec(&cli->refcount);
	local_bh_disable();
	ret = tmem_destroy_pool(pool);
	local_bh_enable();
	kfree(pool);
	pr_info("zcache: destroyed pool id=%d, cli_id=%d\n",
			pool_id, cli_id);
out:
	return ret;
}

/* Create a new mercurial cache pool. One per device. */
static int zcache_new_pool(uint16_t cli_id, uint32_t flags)
{
	int poolid = -1;
	struct tmem_pool *pool;
	struct zcache_client *cli = NULL;

	if (cli_id == LOCAL_CLIENT)
		cli = &zcache_host;
	else if ((unsigned int)cli_id < MAX_CLIENTS)
		cli = &zcache_clients[cli_id];
	if (cli == NULL)
		goto out;
	atomic_inc(&cli->refcount);
	

	/* Allocate metadata for the pool */
	pool = kmalloc(sizeof(struct tmem_pool), GFP_ATOMIC);
	if (pool == NULL) {
		pr_info("zcache: pool creation failed: out of memory\n");
		goto out;
	}

	for (poolid = 0; poolid < MAX_POOLS_PER_CLIENT; poolid++)
		if (cli->tmem_pools[poolid] == NULL)
			break;
	if (poolid >= MAX_POOLS_PER_CLIENT) {
		pr_info("zcache: pool creation failed: max exceeded\n");
		kfree(pool);
		poolid = -1;
		goto out;
	}
	atomic_set(&pool->refcount, 0);
	pool->client = cli;
	pool->pool_id = poolid;
	tmem_new_pool(pool, flags);
	cli->tmem_pools[poolid] = pool;
	printk ("zcache:coldmem project init.\n");
	spin_lock_init(&cold_pages_lock); //ASIM

	/* Actually allocate pool memory */
	zcache_reserve_coldmemory(); //ASIM

	/*	
	for (i =0; i< 20; i++)	{
		printk ("zcache:Got a page %p.\n", zcache_get_coldmempage()); 
	}
	zcache_put_coldmempage(coldmem_pages); 
	*/

	pr_info("zcache: created %s tmem pool, id=%d, client=%d\n",
		flags & TMEM_POOL_PERSIST ? "persistent" : "ephemeral",
		poolid, cli_id);
out:
	if (cli != NULL)
		atomic_dec(&cli->refcount);
	return poolid;
}

/**********
 * Two kernel functionalities currently can be layered on top of tmem.
 * These are "cleancache" which is used as a second-chance cache for clean
 * page cache pages; and "frontswap" which is used for swap pages
 * to avoid writes to disk.  A generic "shim" is provided here for each
 * to translate in-kernel semantics to zcache semantics.
 */

#ifdef CONFIG_CLEANCACHE
static void zcache_cleancache_put_page(int pool_id,
					struct cleancache_filekey key,
					pgoff_t index, struct page *page)
{
	u32 ind = (u32) index;
	struct tmem_oid oid = *(struct tmem_oid *)&key;

	if (likely(ind == index))
		(void)mcache_put_page(LOCAL_CLIENT, pool_id, &oid, index, page);
}

static int zcache_cleancache_get_page(int pool_id,
					struct cleancache_filekey key,
					pgoff_t index, struct page *page)
{
	u32 ind = (u32) index;
	struct tmem_oid oid = *(struct tmem_oid *)&key;
	int ret = -1;

	if (likely(ind == index))
		ret = mcache_get_page(LOCAL_CLIENT, pool_id, &oid, index, page);
	return ret;
}

static void zcache_cleancache_flush_page(int pool_id,
					struct cleancache_filekey key,
					pgoff_t index)
{
	u32 ind = (u32) index;
	struct tmem_oid oid = *(struct tmem_oid *)&key;

	if (likely(ind == index))
		(void)zcache_flush_page(LOCAL_CLIENT, pool_id, &oid, ind);
}

static void zcache_cleancache_flush_inode(int pool_id,
					struct cleancache_filekey key)
{
	struct tmem_oid oid = *(struct tmem_oid *)&key;

	(void)zcache_flush_object(LOCAL_CLIENT, pool_id, &oid);
}

static void zcache_cleancache_flush_fs(int pool_id)
{
	if (pool_id >= 0)
		(void)zcache_destroy_pool(LOCAL_CLIENT, pool_id);
}

static int zcache_cleancache_init_fs(size_t pagesize)
{
	BUG_ON(sizeof(struct cleancache_filekey) !=
				sizeof(struct tmem_oid));
	BUG_ON(pagesize != PAGE_SIZE);
	return zcache_new_pool(LOCAL_CLIENT, 0);
}

static int zcache_cleancache_init_shared_fs(char *uuid, size_t pagesize)
{
	/* shared pools are unsupported and map to private */
	BUG_ON(sizeof(struct cleancache_filekey) !=
				sizeof(struct tmem_oid));
	BUG_ON(pagesize != PAGE_SIZE);
	return zcache_new_pool(LOCAL_CLIENT, 0);
}

static struct cleancache_ops mcache_cleancache_ops = {
	.put_page = zcache_cleancache_put_page,
	.get_page = zcache_cleancache_get_page, // get clears the page from tmem
	.invalidate_page = zcache_cleancache_flush_page,
	.invalidate_inode = zcache_cleancache_flush_inode, //need to check how this is handled.
	.invalidate_fs = zcache_cleancache_flush_fs,
	.init_shared_fs = zcache_cleancache_init_shared_fs,
	.init_fs = zcache_cleancache_init_fs // calls zcache_new_pool
};

struct cleancache_ops zcache_cleancache_register_ops(void)
{
	struct cleancache_ops old_ops =
		cleancache_register_ops(&mcache_cleancache_ops);

	return old_ops;
}
#endif

/*
 * zcache initialization
 * NOTE FOR NOW zcache MUST BE PROVIDED AS A KERNEL BOOT PARAMETER OR
 * NOTHING HAPPENS!
 */

static int zcache_enabled;

static int __init enable_zcache(char *s)
{
	zcache_enabled = 1;
	return 1;
}
__setup("zcache", enable_zcache);

/* allow independent dynamic disabling of cleancache and frontswap */

static int use_cleancache = 1;

static int __init no_cleancache(char *s)
{
	use_cleancache = 0;
	return 1;
}

__setup("nocleancache", no_cleancache);

static int use_frontswap = 1;

static int __init no_frontswap(char *s)
{
	use_frontswap = 0;
	return 1;
}

__setup("nofrontswap", no_frontswap);

static int __init enable_zcache_compressor(char *s)
{
	strncpy(zcache_comp_name, s, ZCACHE_COMP_NAME_SZ);
	zcache_enabled = 1;
	return 1;
}
__setup("zcache=", enable_zcache_compressor);

/*
static int zcache_comp_init(void)
{
	int ret = 0;

	// check crypto algorithm 
	if (*zcache_comp_name != '\0') {
		ret = crypto_has_comp(zcache_comp_name, 0, 0);
		if (!ret)
			pr_info("zcache: %s not supported\n",
					zcache_comp_name);
	}
	if (!ret)
		strcpy(zcache_comp_name, "lzo");
	ret = crypto_has_comp(zcache_comp_name, 0, 0);
	if (!ret) {
		ret = 1;
		goto out;
	}
	pr_info("zcache: using %s compressor\n", zcache_comp_name);

	// alloc percpu transforms 
	ret = 0;
	zcache_comp_pcpu_tfms = alloc_percpu(struct crypto_comp *);
	if (!zcache_comp_pcpu_tfms)
		ret = 1;
out:
	return ret;
}
*/


static int __init zcache_init(void)
{
	int ret = 0;

#ifdef CONFIG_SYSFS
	ret = sysfs_create_group(mm_kobj, &zcache_attr_group);
	if (ret) {
		pr_err("zcache: can't create sysfs\n");
		goto out;
	}
#endif /* CONFIG_SYSFS */
#if defined(CONFIG_CLEANCACHE) || defined(CONFIG_FRONTSWAP)
	if (zcache_enabled) {
		unsigned int cpu;

		tmem_register_hostops(&zcache_hostops);
		tmem_register_pamops(&zcache_pamops);
		ret = register_cpu_notifier(&zcache_cpu_notifier_block);
		if (ret) {
			pr_err("zcache: can't register cpu notifier\n");
			goto out;
		}
		//TODO ret = zcache_comp_init();
		if (ret) {
			pr_err("zcache: compressor initialization failed\n");
			goto out;
		}
		for_each_online_cpu(cpu) {
			void *pcpu = (void *)(long)cpu;
			zcache_cpu_notifier(&zcache_cpu_notifier_block,
				CPU_UP_PREPARE, pcpu);
		}
	}
	/* Here is where the caches are allocted. */
	zcache_objnode_cache = kmem_cache_create("zcache_objnode",
				sizeof(struct tmem_objnode), 0, 0, NULL);
	zcache_obj_cache = kmem_cache_create("zcache_obj",
				sizeof(struct tmem_obj), 0, 0, NULL);
	ret = zcache_new_client(LOCAL_CLIENT);
	if (ret) {
		pr_err("zcache: can't create client\n");
		goto out;
	}
#endif
#ifdef CONFIG_CLEANCACHE
	if (zcache_enabled && use_cleancache) {
		struct cleancache_ops old_ops;

		//TODO inits spinlocks/lists zbud_init();
		INIT_LIST_HEAD(&zbpg_all_list);
		spin_lock_init(&zbpg_all_list_spinlock);
		register_shrinker(&zcache_shrinker);
		old_ops = zcache_cleancache_register_ops();
		pr_info("zcache: cleancache enabled using kernel "
			"transcendent memory and compression buddies\n");
		if (old_ops.init_fs != NULL)
			pr_warning("zcache: cleancache_ops overridden");
	}
#endif
out:
	return ret;
}

module_init(zcache_init)
