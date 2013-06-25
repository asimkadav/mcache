#define COLDMEM_MAP_MAX_PAGES 32768 //128MB pool 
#define MAX_PASR_POOLS_PER_DIMM 8 
#define PASR_PER_POOL_SIZE 4*1024*1024*5


static void *pasr_pools[MAX_PASR_POOLS_PER_DIMM];
static char cold_pages_map[COLDMEM_MAP_MAX_PAGES][MAX_PASR_POOLS_PER_DIMM];
static spinlock_t cold_pages_lock;
static void *coldmem_pages;
extern void zbud_evict_pages(int);

static void * zcache_get_coldmempage(int * poolid)
{

	int i = 0;
	int kmap_fetch = 0;
	static int current_pool = 0;
	int start_pool = current_pool;
	spin_lock(&cold_pages_lock);
	short poolc=0;
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
	
		printk ("Will now try allocating from pool %d.\n", current_pool);
		goto retry_allocation;	
	}


	cold_pages_map[i][current_pool] = 1;
	spin_unlock(&cold_pages_lock);

	 if (poolc >= MAX_PASR_POOLS_PER_DIMM/3) {
                        printk ("some pre-emptive eviction.\n");
                        zbud_evict_pages(poolc*2000);
         }

	*poolid = current_pool;
	return ((void *) page_address((char *)pasr_pools[current_pool]) + i*1024*4);	
	 //return (void *) page_address((i*1024*4 +(char *)pasr_pools[current_pool]));

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

	for (i=0; i< MAX_PASR_POOLS_PER_DIMM; i++)    {
		pasr_pools[i] = NULL;
		for (j=0; j< COLDMEM_MAP_MAX_PAGES; j++)
			cold_pages_map[i][j] = 0;


	}


	for (i=0; i< MAX_PASR_POOLS_PER_DIMM; i++)	{
		pasr_pools[i] = zcache_reserve_coldmemory();
		if (pasr_pools[i] != NULL)	{
			printk ("allocated pool %d: %p \n", i, pasr_pools[i]);
			pasr_kput(pasr_pools[i], 15);
		}
	}


}

void zcache_release_unused_pools(void)
{

	int i,j = 0;
	short pool_busy = 0;

	for (i=0; i< MAX_PASR_POOLS_PER_DIMM; i++)    {
		pool_busy = 0;
		for (j=0; j< COLDMEM_MAP_MAX_PAGES; j++)
			if (cold_pages_map[i][j] == 1) {
				pool_busy = 1;
				break;
			}

 
		if (pool_busy == 0)	{
			if (pasr_pools[i] != NULL)	{
				pasr_kget(pasr_pools[i], 15);
				__free_pages(pasr_pools[i], 15);
				printk ("freed pool %d: %p \n", i, pasr_pools[i]);
			}
		}	
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
		if (pasr_pools[i] != NULL)	{
			pasr_kget(pasr_pools[i], 15);
	                __free_pages(pasr_pools[i], 15);
        	        printk ("freed pool %d: %p \n", i, pasr_pools[i]);
		}	
        }
}
