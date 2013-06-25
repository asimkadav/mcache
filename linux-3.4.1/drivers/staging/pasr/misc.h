#ifndef _MISCDEVICE_H
#define _MISCDEVICE_H

// Driver number
#define MISC_MINOR	45


// Fault injection op-codes
//
#define MCACHE_GET 0x101
#define MCACHE_PUT 0x102
#define REALLY_CORRUPT_STACK 0x103
#define RESET_COUNTS 0x104

#endif
