#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/mman.h>

static int miscdevice_fd = -1;

#define MCACHE_GET 0x101
#define MCACHE_PUT 0x102
#define REALLY_CORRUPT_STACK 0x103
#define RESET_COUNTS 0x104
#define MCACHE_POWER 0x105
#define MCACHE_COMPACT 0x106
#define MCACHE_HOTPLUG 0x107

//
// The idea behind this test is to have the miscdevice notify the user
// daemon to call all the network ethtool functions with symbolic parameters.
// In this case, this test program has no role -- it's simply providing
// a notification.
//
/*
static void test_net(const char *param) {
    int test = atoi (param);
    fprintf (stderr, "Executing test %s/%d\n", param, test);
    
    ioctl (miscdevice_fd, TEST_NET, test);
}
*/


static void mcache_get(const char *dereference)   {
    void *addr;
    int nthflip = atoi(dereference);
    fprintf (stderr, "Executing mcache_get %s/%d\n", dereference, nthflip);
    ioctl (miscdevice_fd, MCACHE_GET, addr);
    printf ("Invoked ioctl MCACHE_GET(%x) for %p..\n", MCACHE_GET, miscdevice_fd);	
}

static void mcache_put(const char *dereference)   {
    int nthflip = atoi(dereference);
    fprintf (stderr, "Executing mcache_put %s/%d\n", dereference, nthflip);
    ioctl (miscdevice_fd, MCACHE_PUT, MCACHE_PUT, nthflip);
}

static void mcache_power ()   {
    fprintf (stderr, "Executing mcache_power \n");
    ioctl (miscdevice_fd, MCACHE_POWER, MCACHE_POWER);
}
static void reallycorrupt_stack(const char *dereference)   {
    int nthflip = atoi(dereference);
    fprintf (stderr, "Executing really corrupt stack %s/%d\n", dereference, nthflip);
    ioctl (miscdevice_fd, REALLY_CORRUPT_STACK, nthflip);
}

static void resetfi()	{
    ioctl (miscdevice_fd, RESET_COUNTS);	
}

int main (int argc, char **argv) {
  
   void * addr; 

  

    if (argc < 3) {
        fprintf (stderr, "Usage:\n");
        fprintf (stderr, "%s <cmd> <OPTION>\n", argv[0]);
        fprintf (stderr, "CMD: get -> mcache get, OPTION: number of pages to get.\n");  
        fprintf (stderr, "CMD: put -> mcache put, OPTION: number of pages to put.\n"); 
        fprintf (stderr, "CMD: mmap -> mcache mmap, OPTION: number of pages to put.\n"); 
        fprintf (stderr, "CMD: power -> mcache power stats, OPTION: number of pages to put.\n"); 
        fprintf (stderr, "CMD: compact -> mcache compact mem, OPTION: number of pages to put.\n"); 
        fprintf (stderr, "CMD: hotplug -> mcache hotplug mem, OPTION: number of pages to put.\n"); 
        return 1;
    }

    miscdevice_fd = open("/dev/mischelp", O_RDWR);
    if (miscdevice_fd == -1) {
        printf ("Failed to open mischelp\n");
        return 2;
    }

    if (strcmp (argv[1], "mmap") == 0) {
	    addr = mmap(4096, 4*1024, PROT_WRITE, MAP_SHARED, miscdevice_fd, 0); 

	    if (addr == NULL)	{
		    printf ("mmap failed.\n");
	    }
	    else	{
		    int i;
		    memset(addr, 'a', 4096);
		    for (i=0;i<4096;i++)	{
			    printf ("%p %c \n ", (char *)addr + i, ((char *)addr )[i]);
		    } 
	    }
     }

    if (strcmp (argv[1], "get") == 0) {
      mcache_get(argv[2]);
    }
	
    if (strcmp (argv[1], "put") == 0) {
      mcache_put(argv[2]);
    }
	
    if (strcmp (argv[1], "power") == 0) {
      mcache_power();
    }
    if (strcmp (argv[1], "compact") == 0) {
      ioctl (miscdevice_fd, MCACHE_COMPACT, MCACHE_COMPACT);
    }

    if (strcmp (argv[1], "hotplug") == 0) {
      ioctl (miscdevice_fd, MCACHE_HOTPLUG, MCACHE_HOTPLUG);
    }
    if (strcmp (argv[1], "reset") == 0) {
	resetfi();	
    }
  

    /*
    if (strcmp (argv[1], "net") == 0) {
        test_net(argv[2]);
    } else if (strcmp (argv[1], "snd") == 0) {
        test_snd(argv[2]);
    } else {
        fprintf (stderr, "Failed:  specify snd or net\n");
    }
    */

    if (miscdevice_fd != -1) {
        close (miscdevice_fd);
    }

    return 0;
}
