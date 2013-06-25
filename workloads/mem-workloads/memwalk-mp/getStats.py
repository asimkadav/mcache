#!/usr/bin/env python

import sys
#import stats
#import pstat
import corestats

vsize = []
rss = []
KB=1024

def getStats(fname):
    fd = open(fname,'r')
    text = fd.readlines()
    fd.close()

    sum = 0.0
    i = 0
    for line in text:
        linesplitlist = line.strip().split(' ')
        vsize.append(float(linesplitlist[0]))
        rss.append(float(linesplitlist[1]))

    vsizestats = corestats.Stats(vsize)
    rssstats = corestats.Stats(rss)
#    print 'mean: '+str(stats.mean(samples))+' median: '+str(stats.median(samples))+' stdev: '+str(stats.lsamplestdev(samples))+' total: '+str(sum)+' 60%: '+str(stats.lscoreatpercentile(samples,0.60))+' 90%: '+str(stats.lscoreatpercentile(samples,0.90))

    print 'vsize: average: '+str(vsizestats.avg()/KB)+' max: '+str(vsizestats.max()/KB)
    print 'rss: average: '+str(rssstats.avg()/KB)+' max: '+str(rssstats.max()/KB)+' median: '+str(rssstats.median()/KB)+' stdev: '+str(rssstats.stdev()/KB)

#    print 'mean: '+str(mystats.avg())+' median: '+str(mystats.median())+' stdev: '+str(mystats.stdev())+' total: '+str(sum)+' 60%: '+str(mystats.percentile(60))+' 90%: '+str(mystats.percentile(90))+' 99%: '+str(mystats.percentile(99.99))+' max: '+str(mystats.max())

getStats(sys.argv[1])
