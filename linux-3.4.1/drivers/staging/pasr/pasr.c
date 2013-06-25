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


#define NPAGES 1


#include "misc.h"
#include "alloc.h"
#include "scm.h"

MODULE_AUTHOR("Asim Kadav");
MODULE_LICENSE("GPL");

#define MAX_PAGES 1048576

static int odft_failure_record = 0;
static struct miscdevice misc_help;

extern void register_mcache_allocate_fn(void * (*odft_fn)(int *) );
extern void register_mcache_free_fn(void * (*odft_fn)(void) );




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

	for (i=0;i<NPAGES;i++)	{
		page = zcache_get_coldmempage(&poolid); //zcache_get_coldmempage();
		printk ("Got address %p.\n", page);
		memset (page, 0, 4096);  
	}

	mcache_print_page(page);
	
	zcache_put_coldmempage(page);

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
	

   if (cmd == REALLY_CORRUPT_STACK)	{
	printk ("Will really corrupt stack %d in next SFI call.\n", arg);
   }

   if (cmd == RESET_COUNTS)	{
	printk ("Reseting fault inejction counts.\n");

   }	



    return 0;
}



struct file_operations misc_fops = {
    .unlocked_ioctl = mischelp_ioctl,
    .owner = THIS_MODULE
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
	
	if (alloc_persistent(PERS_START, 2560) < 0)	
		printk ("Memory offlining failed.\n");
	
	printk ("mcache tester initialized successfully.\n");

	//add_current_pud();
	//memset(PERS_START,0, 4096);

	/*
	for (i=0; i < 2000; i++)	{
		page = zcache_get_coldmempage(&poolid);
	printk ("Allocated %dth page %p..in pool %d.\n", i, page, poolid);
	}
	*/
	//zcache_put_coldmempage(page);

	register_mcache_allocate_fn(&zcache_get_coldmempage);
	register_mcache_free_fn(&zcache_put_coldmempage);
	register_persistent_fault_handler(&do_persistent_fault);
	
	return 0;
}

void cleanup_module(void){
	int number = 0;
	number = misc_deregister(&misc_help);
	if (number < 0) {
		printk ("misc_deregister failed. %d\n", number);
	}

	zcache_release_memory_pools();	
	release_persistent_chunk_all();
	printk ("It's over for mcache tester..\n");
}


void compact_memory(void)
{

;


}










