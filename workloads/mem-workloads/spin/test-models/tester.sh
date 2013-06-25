#!/bin/sh

echo Safety Verification
for i in *.pm 
do   
    echo $i
    spin -a $i > stderr/$i 2>&1
    cc -O3 -DSAFETY -DMEMLIM=10000 -DNSUCC -o pan pan.c 2>> stderr/$i
    ./pan -m10000000 -c0 -n -w27 > stdout/$i 2>> stderr/$i
done
