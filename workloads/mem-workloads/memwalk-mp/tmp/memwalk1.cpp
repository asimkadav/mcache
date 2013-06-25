#define _LARGEFILE64_SOURCE

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <sys/ioctl.h>

using namespace std;

#define _XOPEN_SOURCE 600
#define MEGA 1000000

typedef uint64_t u64;
typedef uint32_t u32;

u64 GetCurrentTime()
{
   struct timeval now;
   gettimeofday( &now, NULL );
   u64 ctime = now.tv_sec;
   ctime *= 1000000;
   ctime += now.tv_usec;
   return ctime;
}

u64 GetCpuCycleCount()
{
   // u64 ts;
   //__asm__ __volatile__( "rdtsc" : "=A" (ts) );
   // return ts;

   // Note: we cannot use "=A" because it would use %rax on x86_64.
   u32 lo, hi;
   __asm__ __volatile__( "rdtsc" : "=a" (lo), "=d" (hi) );
   return (u64)hi << 32 | lo;
}

inline u64 GetCpuFrequency()
{
   char str[400];
   double mhz;
   FILE* fp = fopen("/proc/cpuinfo", "r");
   if( fp )
   {
      while( fscanf( fp, "%[^\n]\n", str ) != EOF )
      {
         if( strncmp( "cpu MHz", str, 7 ) == 0 )
         {
            char *p;
            for( p = &str[7]; p && ( *p < '0' || *p > '9'); ++p );
            mhz = atof(p);
            fclose(fp);
            return (u64)(mhz * 1000000);
         }
      }
      fclose(fp);
   }

   // Either /proc/cpuinfo doesn't exist, or we cannot find cpu
   // frequency from /proc/cpuinfo. So we estimate cpu frequency using
   // GetCpuCycleCount().
   u64 ts1 = GetCpuCycleCount();
   usleep( 4000000 );
   u64 ts2 = GetCpuCycleCount();
   return (ts2 - ts1) >> 2;
}

int generate_rand_offset(int fsize)
{
  //  int offset = rand()%fsize;
  int offset = int((1.0*fsize)*rand()/(RAND_MAX+1.0));
  return offset;  
}

int main(int argc, char *argv[])
{
  int fd;
  char *data;
  u64 cpu_cycle1, cpu_cycle2;
  struct sysinfo s_info;
  struct rusage rbuf;
  int error, err;
  double cpu_freq = double(GetCpuFrequency())/double(MEGA);
  double timeTaken = 0;

  /*
  int misc_fd=open("/dev/fimod",O_RDONLY);
  if(misc_fd==-1) {
    printf("Opening fimod failed,errno:%d\n",errno);
    return -1;
  }
  */

  //  ifstream offset_file;
  //  offset_file.open("offset_file_3.5_full.in");

  //fd = open("test6G.in",O_RDWR);
  fd = -1;
  srand(time(NULL));

  // 1 GiByte
  u64 map_size = 1073741824;
  long long actual_map_size = map_size * 0.125;

  cout << "actual_map_size: " <<  (1.0*actual_map_size)/(1.0*1073741824) << endl;

  if ((data = (char *)mmap64(0, actual_map_size, PROT_READ|PROT_WRITE,MAP_ANON|MAP_SHARED, fd, 0)) == (caddr_t)(-1)) {
    printf("%d,%d\n",errno,ENOMEM);
    perror("mmap");
    //exit(1);
  }


  //  *(data+actual_map_size-2) = 'm';
  //  fsync(fd);
  //printf("pointer [%lx] : [%c] \n",(long)(data+actual_map_size-2),*(data+actual_map_size-2));
  
  // Random - Full Filesize
  int offset = 0;
  long long BIGWINDOW=1<<18;
  long long WINDOW=1<<12;
  long long num_windows = actual_map_size/BIGWINDOW;
  int numOffsets = (1.0*(num_windows-1)*BIGWINDOW)/(WINDOW); // almost 100%
  long long i = 0;
  int count = 0;

  //  long long remainder = actual_map_size%BIGWINDOW;

  for(i=0;i<num_windows-1;i++) {
    memset(data+(i*BIGWINDOW),1,BIGWINDOW);
  }
  
  //memset(data+(num_windows*BIGWINDOW),1,remainder);
  
  char readBuf[WINDOW];
  unsigned int cmd=1;

  unsigned long oldhpgf = 0;
  unsigned long newhpgf = 0;
  unsigned long oldspgf = 0;
  unsigned long newspgf = 0;
  
  timeTaken=0;

  err = getrusage(RUSAGE_SELF,&rbuf);
  oldhpgf = rbuf.ru_majflt;
  oldspgf = rbuf.ru_minflt;

  cpu_cycle1 = GetCurrentTime();
  for(i=0;i<actual_map_size;i+=WINDOW) {
    // offset = generate_rand_offset(numOffsets);
    offset++;
    memcpy(readBuf,data+(offset*WINDOW),WINDOW);
  }
  cpu_cycle2 = GetCurrentTime();
  timeTaken = cpu_cycle2 - cpu_cycle1;

  err = getrusage(RUSAGE_SELF,&rbuf);
  newhpgf = rbuf.ru_majflt;
  newspgf = rbuf.ru_minflt;
  
  cout<<"Orig Time: " << timeTaken << "us, HPGF: " << (newhpgf-oldhpgf) << ", SPGF: " << (newspgf-oldspgf) << endl;

  cout << "sleeping" << endl;
  sleep(10);
  cout << "woken up" << endl;
  
  err = getrusage(RUSAGE_SELF,&rbuf);
  oldhpgf = rbuf.ru_majflt;
  oldspgf = rbuf.ru_minflt;

  timeTaken=0;
  offset=0;
  cpu_cycle1 = GetCurrentTime();
  for(i=0;i<actual_map_size;i+=WINDOW) {
    //offset = generate_rand_offset(numOffsets);
    offset++;
    memcpy(readBuf,data+(offset*WINDOW),WINDOW);
  }
  cpu_cycle2 = GetCurrentTime();
  timeTaken = cpu_cycle2 - cpu_cycle1;

  err = getrusage(RUSAGE_SELF,&rbuf);
  newhpgf = rbuf.ru_majflt;
  newspgf = rbuf.ru_minflt;

  cout<<"Later Time: " << timeTaken << "us, HPGF: " << (newhpgf-oldhpgf) << ", SPGF: " << (newspgf-oldspgf) << endl;

  munmap(data,actual_map_size);
  //  close(fd);
  //  offset_file.close();

  return 0;
}
