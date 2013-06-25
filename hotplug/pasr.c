//asim kadav
//mcache tester

#include <linux/pci.h>
#include <linux/bug.h>
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rtnetlink.h>
#include <linux/lockdep.h>
#include <linux/slab.h>
#include <linux/highmem.h>
#include <linux/cleancache.h>
//#include <linux/pasr.h>

#define NPAGES 1


#include "misc.h"
//#include "alloc.h"
#include "scm.h"

MODULE_AUTHOR("Asim Kadav");
MODULE_LICENSE("GPL");

#define MAX_PAGES 1048576

static int odft_failure_record = 0;
static struct miscdevice misc_help;

extern void register_mcache_allocate_fn(void * (*odft_fn)(int *) );
extern void register_mcache_free_fn(void * (*odft_fn)(void) );
extern void register_mcache_pool_free_fn(void (*odft_fn)(void) );
//extern int pasr_power(void);
void compact_memory(void);
void hotplug_memory(void); 

/*
 * Goals: To get memory allocation working
 * 2. To get hotplug working 
 * 3. Export a hook
 *
 */

void mcache_print_page (char * page)
{
	int i = 0;
	for (i=0; i< 4096; i++)	
		printk ("%c ", page[i]);
	printk ("\n\n");

}


void mcache_get_pages (int npages)  {
	void *page = NULL;
	int i = 0;
	int poolid = 0;

	if ((npages < 0) || (npages > MAX_PAGES))	{
		printk ("mcache_test:mcache_get_pages: Page count buggy.\n");
		return;
	}	

	//zcache_get_coldmempage(&poolid);

	return; 
}

void mcache_put_pages (int npages )  {

	void *page = NULL;
	int i = 0;

	printk ("mcache_test:mcache_put_pages: Page count buggy.\n");	

	return;
}

int odft_stack_to_really_corrupt(void)  {
   return 0;
}


static int mischelp_ioctl(struct inode *inode, struct file *fp,
                          unsigned int cmd, unsigned long arg) {
    //
    // Note:  Use to toggle different types of fault inejection.
    //
   
   //    char *alldata = MJR_alloc(12);
   //    struct req_args args;
   //    memset (alldata, 0, sizeof (unsigned char) * 12);
   //    memcpy (&alldata[0], &cmd, sizeof (unsigned int));
   //    memcpy (&alldata[4], &arg, sizeof (unsigned long));
  
   printk ("Received an ioctl %x %x.\n", cmd, arg); 

   if (cmd == MCACHE_GET)    {
      printk ("Will get mcache %d pages.\n", (int) arg);
      mcache_get_pages ((int) arg);	 
   }
    
   if (cmd == MCACHE_PUT)	{
	printk ("Will put mcache %d pages.\n", (int) arg);
	mcache_put_pages ((int) arg);
   }
	
   
   if (cmd == MCACHE_COMPACT)	{
	printk ("Will compact memory..\n");	
	compact_memory();
   }	

   if (cmd == MCACHE_HOTPLUG) {

	printk ("Will hotplug memory..\n");	
	hotplug_memory();

    }

    return 0;
}

/*
void remap_page(unsigned long address)
	pgd_t *pgd;
        pud_t *pud;
        pmd_t *pmd;
        pte_t *pte;
        pte_t entry;
        pte_t *page_table;
        struct mm_struct *mm;
        spinlock_t *ptl;
        unsigned long page_nr;

        mm = current->mm;

        //spin_lock(&mlock);
        pgd = pgd_offset(mm, address);
        pud = pud_alloc(mm, pgd, address);
        if (!pud)
                return PROTFAIL;
        pmd = pmd_alloc(mm, pud, address);
        if (!pmd)
                return PROTFAIL;
        if (unlikely(pmd_none(*pmd)) && __pte_alloc(mm, NULL, pmd, address))
                return PROTFAIL;

        pte = pte_offset_map(pmd, address);

        page_table = pte_offset_map_lock(mm, pmd, address, &ptl);

        page_nr = vmalloc_to_pfn(pasr_pools[0]);
        entry = __pte((phys_addr_t)page_nr << PAGE_SHIFT);
        entry = pte_mkuser_accessible(pte_mkpresent(entry));
        set_pte_at(mm, address, page_table, entry);

        pte_unmap_unlock(page_table, ptl);

        flush_tlb_others(cpu_online_mask, NULL, address);
        printk(KERN_ERR"pte modified entry %lx", entry);
        //spin_unlock(&mlock);

        *(unsigned long *)page = address;
        printk(KERN_ERR"monitor page %pK mapped at %lx", monitor_page, address);
        return 0;
}
*/


static int mischelp_mmap(struct file *file, struct vm_area_struct *vma)
{
	int poolid = 0;
        size_t size = vma->vm_end - vma->vm_start;

	printk ("In mischelp_mmap.\n");

        if ((size > PAGE_SIZE) || (vma->vm_pgoff != 0))
                return -EINVAL;

	//remap_page(vma->vm_start);

  //      if (remap_pfn_range(vma, vma->vm_start,
    //                        vmalloc_to_pfn(zcache_get_coldmempage(&poolid)) >> PAGE_SHIFT,
      //                      size, vma->vm_page_prot))
        //        return -EAGAIN;

        return 0;
}


struct file_operations misc_fops = {
    .unlocked_ioctl = mischelp_ioctl,
    .owner = THIS_MODULE,
    .mmap = mischelp_mmap,
};




int init_module(void){
	int retval = 0;
	static char *mischelp_name = "mischelp";	
	void *page = NULL;
	int poolid = 0;
	int i = 0;

	misc_help.minor = MISC_MINOR;
	misc_help.name = mischelp_name;
	misc_help.fops = &misc_fops;
	retval = misc_register(&misc_help);

	if (retval)
		return retval;


	init_scm();
	//zcache_create_memory_pools();
	
	//if (alloc_persistent(PERS_START, 128) < 0)	
	//	printk ("Memory offlining failed.\n");
	
	printk ("mcache tester initialized successfully.\n");


	/*
	for (i=0; i < 2000; i++)	{
		page = zcache_get_coldmempage(&poolid);
	printk ("Allocated %dth page %p..in pool %d.\n", i, page, poolid);
	}
	*/
	//zcache_put_coldmempage(page);

	//register_mcache_allocate_fn(&zcache_get_coldmempage);
	//register_mcache_free_fn(&zcache_put_coldmempage);
	//register_mcache_pool_free_fn(&zcache_release_unused_pools);
	//register_persistent_fault_handler(&do_persistent_fault);
	
	return 0;
}

void cleanup_module(void){
	int number = 0;
	number = misc_deregister(&misc_help);
	if (number < 0) {
		printk ("misc_deregister failed. %d\n", number);
	}

	//zcache_release_memory_pools();	
	release_persistent_chunk_all();
	printk ("It's over for mcache tester..\n");
}


void compact_memory()
{
	unsigned long p_index;
	 
	while  (allocate_persistent_chunk(&p_index, 128)  >= 0) 
		printk ("Memory offlining failed.\n");
	release_persistent_chunk_all();	

}

void hotplug_memory() 
{
	unsigned long p_index;
	int rc = 0;

    /*
	rc = allocate_persistent_chunk(&p_index, 1024);	
	while  (rc  >= 0)	{
		rc = allocate_persistent_chunk(&p_index, 1024);
		printk ("Offlining memory.\n");
	}

	while  (rc  >= 0)	{
		rc = allocate_persistent_chunk(&p_index, 512);
		printk ("Offlining memory.\n");
	}

	while  (rc  >= 0)	{
		rc = allocate_persistent_chunk(&p_index, 256);
		printk ("Offlining memory.\n");
	}
    
     */ 

	while  (rc  >= 0)	{
		rc = allocate_persistent_chunk(&p_index, 128);
		printk ("Offlining memory.\n");
	}


	while  (rc  >= 0)	{
		rc = allocate_persistent_chunk(&p_index, 64);
		printk ("Offlining memory.\n");
	}

	//release_persistent_chunk_all();
}

