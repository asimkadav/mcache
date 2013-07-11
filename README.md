Mercurial Cache
===============

Contains code for mercurial cache implementation. The linux tree contains
the zcache implementation and VM integration while the pasr directory has 
code to detect working sets, automatically claim and free memory and de-fragment
memory space.

/linux-3.4.1 : The kernel with support for memory identication, physical-real mapping and zcache support.

/pasr : the kernel module to claim/free memory as working sets increase/decrease.

To build linux-3.4.1, refer to http://www.cyberciti.biz/tips/compiling-linux-kernel-26.html on how to compile and build your own kernel.

