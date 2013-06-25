# This makefile builds both the miscellaneous device, and the custom PCI bus driver.

KERNEL_VERSION = linux-3.4.1 
# DEBUG = y

#EXTRA_CFLAGS += -fno-inline -Os -g -Wall $(DEFINE_ENABLE_UPRINTK)
EXTRA_CFLAGS += -Wno-attributes 

obj-m := mcache.o \
#	module-format.o 
#	musb-core.o \
#	musb-gadget.o \
#	musb-bulk.o \
#	musb-net.o \
#	musb-sound.o

# Misc device, provides all the glue code for communication and object tracking
mcache-objs := pasr.o scm.o 
#pasr-objs += scm.o
# The idea is to check if the reboot will give us the
# "Invalid module format" error or not.

all:
	$(MAKE) -C /scratch/mcache/$(KERNEL_VERSION) M=`pwd` modules
	#gcc $(EXTRA_FLAGS) mischelper.c -o mischelper.o
	#gcc mischelper.o -o mischelper
	gcc mischelper.c 
	rm -f mischelper
	mv a.out mischelper
	chmod 555 mischelper

clean:
	rm -f *.o *.ko *.mod.c Module.symvers .mischelp.* .misc.o.d modules.order
	rm -rf ./.tmp_versions
	rm -f \.*.cmd
	rm -f mischelper
	rm -rf ./scm/*.o
