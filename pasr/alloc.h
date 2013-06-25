#define COLDMEM_MAP_MAX_PAGES 32768 //128MB pool 
#define MAX_PASR_POOLS_PER_DIMM 128
#define PASR_PER_POOL_SIZE 4*1024*1024*5


static void *pasr_pools[MAX_PASR_POOLS_PER_DIMM];
static char cold_pages_map[COLDMEM_MAP_MAX_PAGES][MAX_PASR_POOLS_PER_DIMM];
static spinlock_t cold_pages_lock;
static spinlock_t resize_pages_lock;
static void *coldmem_pages;
extern void zbud_evict_pages(int);
extern void zbud_evict_pool_pages(int, int);
extern void zcache_drop_all_caches();
extern unsigned long zcache_get_and_reset_copy_count();
static int current_pool = 0;
void zcache_expand_memory_pools(void);
int zcache_release_unused_pool(void);
unsigned long zcache_used_memory_count(void);
static atomic_t zcache_reclaim_flag;
static short zcache_initializing = 0;
static short pasr_fraction = 2;
static struct task_struct *zcache_tsk = NULL;
static DECLARE_WAIT_QUEUE_HEAD(zcache_wait);


static int ZCACHE_TIMEOUT = HZ*5;

/* protetected by cold pages lock */
static unsigned long nr_zcache_free_pages = 0;
static unsigned long nr_zcache_max_free_pages = 0;
static unsigned long nr_zcache_used_pages = 0;

int zcache_grab_unused_pool(void);
void pasr_memory(int *low, int * high);

static void * zcache_get_coldmempage(int * poolid)
{

	int i = 0;
    void *page;
	int kmap_fetch = 0;
	//static int current_pool = 0;
	int start_pool = current_pool;
	short poolc=0;
	spin_lock(&cold_pages_lock);
retry_allocation:
	i = 0;

	
	while (cold_pages_map[i][current_pool] != 0)  {
		i++;
		if (i >= COLDMEM_MAP_MAX_PAGES)
			break;
	}

	if ((i >= COLDMEM_MAP_MAX_PAGES) || (pasr_pools[current_pool] == NULL))   {
		poolc ++;
		//printk ("zcache: memory allocation pool full.\n");
		current_pool= (current_pool+1)%MAX_PASR_POOLS_PER_DIMM;

		if (current_pool == start_pool)	{
			printk ("zcache:memory allocation failed.\n");
			spin_unlock(&cold_pages_lock);
			if (poolc >= MAX_PASR_POOLS_PER_DIMM/3) {
				printk ("some pre-emptive eviction.\n");
				zbud_evict_pages(poolc*2000);
			}
			
			return NULL;	
		}
	
		//printk ("Will now try allocating from pool %d.\n", current_pool);
		goto retry_allocation;	
	}


	cold_pages_map[i][current_pool] = 1;
	nr_zcache_used_pages++;
	nr_zcache_free_pages--;
	spin_unlock(&cold_pages_lock);

	 if ((zcache_used_memory_count() > MAX_PASR_POOLS_PER_DIMM) && (poolc >= MAX_PASR_POOLS_PER_DIMM/3)) {
                        printk ("some pre-emptive eviction.\n");
                        zbud_evict_pages(poolc*2000);
         }

	*poolid = current_pool;
    page = page_address((char *)pasr_pools[current_pool]) + i*4096;
    BUG_ON(page == NULL);
    return page;
	//return ((void *) page_address((char *)pasr_pools[current_pool]) + i*4096);	

}


static void * zcache_reserve_coldmemory(void)
{
        //void * pages = (void *)alloc_pages (GFP_HIGHUSER, 14);
        void * pages = (void *)alloc_pages (GFP_HIGHUSER_MOVABLE, 15);
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


	for (poolid = 0; poolid < MAX_PASR_POOLS_PER_DIMM; poolid++)	{

		if (pasr_pools[poolid] == NULL)
			continue;	

		if ((((char *) addr - (char *) page_address(pasr_pools[poolid])) < 0 ) || 
				(((char *) addr - (char *) page_address(pasr_pools[poolid]))/4096 >   COLDMEM_MAP_MAX_PAGES))	{
			if (poolid >= MAX_PASR_POOLS_PER_DIMM)	{
				free_page(addr);
				return;
				//printk ("WARN: Freeing unaccounted memory %p from poolid %d..\n", addr, poolid);
				//return NULL;
			}
			continue;
		}


		index = ((char *) addr - (char *)page_address(pasr_pools[poolid]) )/4096;


		//printk ("I just put page %p from pool %d index %d.\n", addr, poolid, index);	
		if ((index <0) || (index >= COLDMEM_MAP_MAX_PAGES))	{
			printk ("put_coldmempage failed.\n");
			return;
		}

		spin_lock(&cold_pages_lock);
		nr_zcache_used_pages--;
		nr_zcache_free_pages++;	
		cold_pages_map[index][poolid] = 0;
		//printk ("I just put page %p from pool %d index %d.\n", addr, poolid, index);
		spin_unlock(&cold_pages_lock);
		return;
	}
	return;

}

/*
  * 16-bit Linear Feedback Shift Register (LFSR)
  *
  *                       16   14   13    11
  * Feedback polynomial = X  + X  + X  +  X  + 1
  */
 static unsigned int lfsr_random(void)
 {
     static unsigned int lfsr_value = 0xF00D;
     unsigned int bit;
 
     /* Compute next bit to shift in */
     bit = ((lfsr_value >> 0) ^
            (lfsr_value >> 2) ^
            (lfsr_value >> 3) ^
            (lfsr_value >> 5)) & 0x0001;
 
     /* Advance to next register value */
     lfsr_value = (lfsr_value >> 1) | (bit << 15);
 
     return lfsr_value;
 }
 




void zcache_corrupt_pools()
{
  // Corrupt pools at specific probability..
  // About 85 random paegs every 128MB pool.
 int i;
  

  for (i=0; i< MAX_PASR_POOLS_PER_DIMM; i++)    {
	  if (pasr_pools[i] != NULL)      {
		  int p_index = 0;
	          p_index = lfsr_random()%COLDMEM_MAP_MAX_PAGES;
		  int p_size = lfsr_random()%PAGE_SIZE;			  		 
		  memset((page_address(pasr_pools[i]+ 4*1024*p_index)), 99, (size_t)p_size);  
	  }


  }

}


void zcache_create_memory_pools(void)
{

        int i,j = 0;

	zcache_initializing = 1;

	for (i=0; i< MAX_PASR_POOLS_PER_DIMM; i++)    {
		pasr_pools[i] = NULL;
		for (j=0; j< COLDMEM_MAP_MAX_PAGES; j++)
			cold_pages_map[j][i] = 0;
	}
	
	for (i=0; i< MAX_PASR_POOLS_PER_DIMM - 2; i++)	{ // test remove 2..
		pasr_pools[i] = zcache_reserve_coldmemory();
		if (pasr_pools[i] != NULL)	{
			nr_zcache_free_pages += COLDMEM_MAP_MAX_PAGES;
			printk ("allocated pool %d: %p \n", i, pasr_pools[i]);
			pasr_kput(pasr_pools[i], 15);
		}
	}

	zcache_initializing = 0;
}

unsigned long zcache_total_memory()
{
	return (nr_zcache_free_pages + nr_zcache_used_pages);
}
unsigned long zcache_used_memory_count()
{
	return nr_zcache_used_pages;
}

unsigned long zcache_free_memory_count()
{
   // if (!nr_zcache_max_free_pages)
    return (nr_zcache_free_pages + nr_zcache_used_pages);

  //  else 
    //    return nr_zcache_free_pages/2;

   // else return (zcache_total_memory()/100)* (100 - 75*zcache_total_memory()/nr_zcache_max_free_pages);
}



// Return free memory count in pages
unsigned long zcache_used_memory()
{

	int page_size= 4096;
	int current_pasr_pool = 0, i =0;
	unsigned long free_pages = 0;
	for (current_pasr_pool=0; current_pasr_pool< MAX_PASR_POOLS_PER_DIMM; current_pasr_pool++)    {
	   if (pasr_pools[current_pasr_pool] != NULL)	
		for (i = 0; i < COLDMEM_MAP_MAX_PAGES; i ++)	{
			if (cold_pages_map[i][current_pasr_pool] == 0)  {
				free_pages++;
			}
		}
	}
	return free_pages;
}


void zcache_resize(void)
{
	static unsigned long last_watermark = 0;
	static int compact_strike = 0;
	int grabbed_pages = 0;
    int low = 0;
    int high = 0;
    int count = 0;
	unsigned long free_pages = 0;
    short should_free_page_cache = 1;
	do {

		wait_event_interruptible_timeout(zcache_wait, ((nr_free_pages() - zcache_free_memory_count() >= COLDMEM_MAP_MAX_PAGES*2) && (nr_free_pages() > last_watermark + COLDMEM_MAP_MAX_PAGES)) || kthread_should_stop(), ZCACHE_TIMEOUT);

		if (zcache_initializing)
			continue;

		if (kthread_should_stop())
			break;

		free_pages = nr_free_pages() - zcache_free_memory_count();

		if (free_pages > last_watermark + COLDMEM_MAP_MAX_PAGES*2){	

			if (free_pages >= COLDMEM_MAP_MAX_PAGES*2)   {
                atomic_set (&zcache_reclaim_flag, 1);
				grabbed_pages = zcache_grab_unused_pool();
            }    
			else
				printk ("few pages left..%lu.\n", nr_free_pages());



			if (grabbed_pages <= 0)	{
				atomic_set (&zcache_reclaim_flag, 1);
                if ((last_watermark == 0) || (should_free_page_cache == 0))    {
    				last_watermark = free_pages;
				    printk ("****LAST WATERMARK SET AS %lu .\n", last_watermark);
                }
                else {
                    if (should_free_page_cache == 1)    {
                            printk ("zcache:page cache evicted..\n");
                            zcache_drop_all_caches();
                    }        
                    should_free_page_cache = 0;
                }
                nr_zcache_max_free_pages = zcache_total_memory();
				//wake_up_process(zcache_tsk);
			}

		}

		// Free memory.
		else	{
			if (free_pages < (last_watermark - COLDMEM_MAP_MAX_PAGES))	{
				printk ("zcache_resize..will free pages.\n");
				//atomic_set (&zcache_reclaim_flag, 1);
                should_free_page_cache = 1;
				zcache_release_unused_pool();
			}

		}

		
		// De-fragment memory space if last allocation failed at large memory space
		// and there is still lots of free space.

		if ((last_watermark > 500000) && (free_pages > 200000))	{
			compact_strike++;
			if (compact_strike == 3)	{	
                printk ("Memory is fragmented issuing compaction..\n");
                compact_memory();
                last_watermark = 0;
				compact_strike = 0;
			}
		}		


		atomic_set (&zcache_reclaim_flag, 0);

        low = 0; high = 0;
        pasr_memory(&low, &high);

                
        printk ("low:high:copy:total %lu:%lu:%lu\n", low, high, zcache_get_and_reset_copy_count());

        
		printk ("zcache_resize: current_pool %d free_pages:%lu grabbed_pages: %d last_watermark %lu free mem: %lu used mem:%lu.\n max free:%lu\n"  // reported: %lu.. zcache_Free:%lu \n", 
						,current_pool, free_pages, grabbed_pages, last_watermark, zcache_free_memory_count(), zcache_used_memory_count(), nr_zcache_max_free_pages); //(nr_zcache_free_pages/100)* (100 - 50*zcache_total_memory()/nr_zcache_max_free_pages), nr_zcache_free_pages   );
        
                  

#ifdef ZCACHE_LOWREFRESH
        // DO corruption
        
        //43 errors per pool 
        //
        for (count=0; count<43;count++) {
            char *buf = page_address(pasr_pools[current_pool]);
            buf[jiffies%COLDMEM_MAP_MAX_PAGES] = 0;
        }    

#endif


		msleep(15000); //COMMENTED to get compaction stats..
	}	while (1);
	
}
/*

void zcache_resize_wait(void)
{

	spin_lock (&resize_pages_lock);
	wake_up_interruptible(&kapmd_wait);
	mod_timer(&zcache_timer);
	spin_unlock(&zcache_pages_lock);
}
*/


int zcache_grab_unused_pool(void)
{

        int i,j = 0;
        short pool_slot_free = 0;

        for (i=0; i< MAX_PASR_POOLS_PER_DIMM; i++)    {
                pool_slot_free = 0;
	
		if (pasr_pools[i] == NULL)	
			pool_slot_free = 1;

        if (pool_slot_free == 1)     {
            atomic_set(&zcache_reclaim_flag,1);
            pasr_pools[i] = zcache_reserve_coldmemory();
            if (pasr_pools[i] != NULL)      {
                //__free_pages(pasr_pools[i], 15);
                nr_zcache_free_pages += COLDMEM_MAP_MAX_PAGES;	
                pasr_kput(pasr_pools[i], 15);
                printk ("alloc'ed mercurial pool %d \n", i);
                spin_lock(&cold_pages_lock);
                for (j=0; j< COLDMEM_MAP_MAX_PAGES; j++)
                    cold_pages_map[j][i] = 0;
                atomic_set (&zcache_reclaim_flag, 0);
                spin_unlock(&cold_pages_lock);
                return COLDMEM_MAP_MAX_PAGES; //FREES ONLY ONE POOL
            }
            //else zcache_drop_all_caches(); // THIS IS DANGEROUS. NEED TO CHECK SYSTEM IDLE USAGE TOO.
        }
        }

	atomic_set (&zcache_reclaim_flag, 0);
	return -1;
}

int zcache_release_unused_pool(void)
{

        int i,j = 0;
        short pool_busy = 0;

	if (atomic_read(&zcache_reclaim_flag) == 1)	{
		//printk ("zcache is reclaiming..\n");
		return -2;
	}

	spin_lock(&cold_pages_lock);
        for (i=0; i< MAX_PASR_POOLS_PER_DIMM; i++)    { // CHANGE it back to 0

                if (pasr_pools[i] == NULL)
                    continue;

                pool_busy = 0;
                for (j=0; j< COLDMEM_MAP_MAX_PAGES; j++)  {
                        if (cold_pages_map[j][i] == 1) {
                                pool_busy = 1;
				//printk ("Pool:%d busy at %d..\n", i, j);
                                break;
                        }
                }

                if (pool_busy == 0)     {
                        if (pasr_pools[i] != NULL)      {
                                //__free_pages(pasr_pools[i], 15);
                                pasr_kget(pasr_pools[i], 15);
				mb();
                                __free_pages(pasr_pools[i], 15);
                                pasr_pools[i] = NULL;
				nr_zcache_free_pages -= COLDMEM_MAP_MAX_PAGES;
                                printk ("freed mercurial pool %d \n", i);
                                mb();
				spin_unlock(&cold_pages_lock);
                                return COLDMEM_MAP_MAX_PAGES; //FREES ONLY ONE POOL
                        }
                }
        }
	spin_unlock(&cold_pages_lock);
    if (pool_busy  == 1) {
        printk ("******resetting back pasr fraction..\n");
        pasr_fraction  = 1;
    }    
    return -1;
}





void zcache_release_memory_pools(void)
{

	 int i,j = 0;

        for (i=0; i< MAX_PASR_POOLS_PER_DIMM; i++)    {
                for (j=0; j< COLDMEM_MAP_MAX_PAGES; j++)
                        cold_pages_map[j][i] = 0;
        }


        for (i=0; i< MAX_PASR_POOLS_PER_DIMM; i++)    {
		if (pasr_pools[i] != NULL)	{
			pasr_kget(pasr_pools[i], 15);
	                __free_pages(pasr_pools[i], 15);
        	        //printk ("freed pool %d: %p \n", i, pasr_pools[i]);
		}	
        }
}
