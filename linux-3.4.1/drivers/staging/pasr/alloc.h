#define COLDMEM_MAP_MAX_PAGES 1024
#define MAX_PASR_POOLS_PER_DIMM 1 


static void *pasr_pools[MAX_PASR_POOLS_PER_DIMM];
static char cold_pages_map[COLDMEM_MAP_MAX_PAGES][MAX_PASR_POOLS_PER_DIMM];
static spinlock_t cold_pages_lock;
static void *coldmem_pages;


static void * zcache_get_coldmempage(int * poolid)
{

	int i = 0;
	int kmap_fetch = 0;
	static int current_pool = 0;
	int start_pool = current_pool;
	spin_lock(&cold_pages_lock);

retry_allocation:
	i = 0;

	while (cold_pages_map[i][current_pool] != 0)  {
		i++;
		if (i >= COLDMEM_MAP_MAX_PAGES)
			break;
	}

	if ((i >= COLDMEM_MAP_MAX_PAGES) || (pasr_pools[current_pool] == NULL))   {
		printk ("zcache: memory allocation pool full.\n");
		current_pool= (current_pool+1)%MAX_PASR_POOLS_PER_DIMM;
		if (current_pool == start_pool)	{
			printk ("zcache:memory allocation failed.\n");
			return NULL;	
		}
		printk ("Will now try allocating from pool %d.\n", current_pool);
		goto retry_allocation;	
	}


	cold_pages_map[i][current_pool] = 1;
	spin_unlock(&cold_pages_lock);

	*poolid = current_pool;
	return ((void *) page_address((char *)pasr_pools[current_pool]) + i*1024*4);	
	 //return (void *) page_address((i*1024*4 +(char *)pasr_pools[current_pool]));

}


static void * zcache_reserve_coldmemory(void)
{
        //void * pages = (void *)alloc_pages (GFP_HIGHUSER, 14);
        void * pages = (void *)alloc_pages (GFP_KERNEL, 15);
	int i = 0;

	
	if (pages == NULL)	{
		printk("Memory allocation failed.\n");
        	return NULL;
	}

	coldmem_pages = pages;
	
        return pages;
}

static void zcache_put_coldmempage(void * addr)
{

	int poolid =0;
	int index = 0;

	while (pasr_pools[poolid] == NULL)	{
		poolid++;
		if (poolid >= MAX_PASR_POOLS_PER_DIMM)  {
			printk ("WARN: Freeing unaccounted memory..\n");
			return NULL;
		}
	}


	while ((((char *) addr - (char *) page_address(pasr_pools[poolid])) < 0 ) || 
			(((char *) addr - (char *) page_address(pasr_pools[poolid]))/4096 >   COLDMEM_MAP_MAX_PAGES))	{
		poolid++;
		if (poolid >= MAX_PASR_POOLS_PER_DIMM)	{
			printk ("WARN: Freeing unaccounted memory %p from poolid %d..\n", addr, poolid);
			return NULL;
		}
	}


        index = ((char *) addr - (char *)page_address(pasr_pools[poolid]) )/4096;


	//printk ("I just put page %p from pool %d index %d.\n", addr, poolid, index);	
        if ((index <0) || (index >= COLDMEM_MAP_MAX_PAGES))	{
		printk ("put_coldmempage failed.\n");
                return;
	}

        spin_lock(&cold_pages_lock);
        cold_pages_map[index][poolid] = 0;
        spin_unlock(&cold_pages_lock);
        return;
}

void zcache_lowmem_release_pool(void * addr)
{




}



void zcache_create_memory_pools(void)
{

        int i,j = 0;

	for (i=0; i< MAX_PASR_POOLS_PER_DIMM; i++)    {
		pasr_pools[i] = NULL;
		for (j=0; j< COLDMEM_MAP_MAX_PAGES; j++)
			cold_pages_map[i][j] = 0;


	}


        for (i=0; i< MAX_PASR_POOLS_PER_DIMM; i++)	{
                pasr_pools[i] = zcache_reserve_coldmemory();
		printk ("allocated pool %d: %p \n", i, pasr_pools[i]);
	}


}

void zcache_release_memory_pools(void)
{

	 int i,j = 0;

        for (i=0; i< MAX_PASR_POOLS_PER_DIMM; i++)    {
                for (j=0; j< COLDMEM_MAP_MAX_PAGES; j++)
                        cold_pages_map[i][j] = 0;
        }


        for (i=0; i< MAX_PASR_POOLS_PER_DIMM; i++)    {
                __free_pages(pasr_pools[i], 15);
                printk ("freed pool %d: %p \n", i, pasr_pools[i]);
			
        }


}
