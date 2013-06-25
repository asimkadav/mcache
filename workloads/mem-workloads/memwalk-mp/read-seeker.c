#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <sys/time.h>
#include <sys/types.h>

#define INIT_OFFSET 100
//#define BLOCKSIZE 65536
//#define BLOCKSIZE 1048576
//#define BLOCKSIZE 6291456
#define BLOCKSIZE 32768
//#define BLOCKSIZE 131072
//#define BLOCKSIZE 4096
#define PAGESIZE 4096
//#define NUMPAGES 32
#define NUMPAGES 8
#define SEEKDIST 10485760
#define NUMSEEKS 0
//#define BLOCKSIZE 262144
//#define FILESIZE 2147483648
//#define FILESIZE 734003200
//#define FILESIZE 8589934592
//#define FILESIZE 10737418240
//#define FILESIZE 3221225472
#define FILESIZE 4294967296
//#define FILESIZE 6442450944
#define TIMEOUT 1000
#define MEGA 1048576

int count;
time_t start;


unsigned long GetCurrentTime()
{
  struct timeval now;
  gettimeofday( &now, NULL );
  unsigned long ctime = now.tv_sec;
  ctime *= 1000000;
  ctime += now.tv_usec;
  return ctime;
}


void done()
{
	time_t end;

	time(&end);

	if (end < start + TIMEOUT) {
	        printf(".");
		alarm(1);
		return;
	}

	if (count) {
	  printf("\nResults: IO: %d, IOPS: %d, B/W: %f MBps\n",count,count/TIMEOUT,(1.0*count*BLOCKSIZE)/(1.0*MEGA*TIMEOUT));

	  //	  printf(".\nResults: %d seeks/second, %.2f ms random access time\n",
	  //		 count / TIMEOUT, 1000.0 * TIMEOUT / count);
	}
	exit(EXIT_SUCCESS);
}

void handle(const char *string, int error)
{
	if (error) {
		perror(string);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv)
{
	char buffer[BLOCKSIZE];
	int fd, retval;
	unsigned long numblocks;
	off64_t offset;

	setvbuf(stdout, NULL, _IONBF, 0);

	//	printf("Seeker v2.0, 2007-01-15, "
	//	       "http://www.linuxinsight.com/how_fast_is_your_disk.html\n");

	/*
	if (argc != 2) {
		printf("Usage: seeker <raw disk device>\n");
		exit(EXIT_SUCCESS);
	}
	*/

	//fd = open("/mnt/ssd-ibm/tmp1.out", O_WRONLY|O_CREAT);
	//	fd = open("/mnt/ssd-ibm/tmp1.out", O_WRONLY|O_CREAT);
	// fd = open("/dev/sdc2", O_WRONLY);
	//fd = open("/mnt/ssd-int1/test.dat", O_WRONLY);
	//fd = open("/mnt/ssd-int2", O_WRONLY|O_CREAT);
	//	fd = open("/dev/sdb2", O_RDONLY);
	//fd = open("/dev/sdb2", O_RDONLY);
	//	fd = open("/media/windows/tmp/tmp.dat", O_WRONLY);
	fd = open("/media/windows/tmp/tmp.dat", O_RDONLY);

	handle("open", fd < 0);

	/*
	if ((data = (char *)mmap64(0, actual_map_size, PROT_READ|PROT_WRITE,MAP_FILE, fd, 0)) == (caddr_t)(-1)) {
	  printf("%d,%d\n",errno,ENOMEM);
	  perror("mmap");
	  //exit(1);                                                                                                                                                                                       
	}
	*/

	//	retval = ioctl(fd, BLKGETSIZE, &numblocks);
	//	handle("ioctl", retval == -1);
	//	numblocks = 6442450944/BLOCKSIZE;
	numblocks = FILESIZE/BLOCKSIZE;

	//	printf("Benchmarking %s [%luMB], wait %d seconds",
	//	       argv[1], numblocks / 2048, TIMEOUT);

	time(&start);
	srand(start);
	signal(SIGALRM, &done);
	alarm(1);
	//offset = INIT_OFFSET*BLOCKSIZE;
	offset = 0;

	fsync(fd);
	//	posix_fadvise(fd,0,0,POSIX_FADV_DONTNEED);

	off64_t off = 0;
	//off64_t init = MEGA*1024;
	off64_t init = 0;
	int i=0;
	unsigned long t1,t2;
	int l =0;
	int REPEAT=5;

	while(l<REPEAT) {
	  offset=0;
	  while(offset<FILESIZE-BLOCKSIZE) {
	    offset += PAGESIZE;

	    //	    printf("read\n");
	    //retval = write(fd, buffer, PAGESIZE);
	    retval = read(fd, buffer, PAGESIZE);
	    
	    handle("write", retval < 0);
	    count++;
	  }
	  retval = lseek64(fd, 0, SEEK_SET);
	  handle("lseek", retval < 0);
	  l++;
	}
	
	printf("File exceeded.\n");
	time_t time_taken;
	time(&time_taken);
	time_taken = time_taken-start;
	printf("\nResults: Time: %d, IO: %d, IOPS: %d, B/W: %f MBps\n",time_taken,count,count/time_taken,(1.0*count*BLOCKSIZE)/(1.0*MEGA*time_taken));
	return 1;
	/* notreached */
}
