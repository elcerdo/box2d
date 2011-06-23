#!/usr/bin/env python2
# coding: utf-8

import os 
import sys 
import cPickle as pickle
import shutil

from pylab import *

rundir = sys.argv[1]
results = []
for root,dirs,files in os.walk(rundir):
    results.extend(os.path.join(root,filename) for filename in files if 'pck' in filename)

print "found %d results" % len(results)

definitions = []
kkmax = len(results)
for kk,filename in enumerate(results):
    print "%d/%d %s" % (kk,kkmax,filename)
    data = pickle.load(file(filename))
    assert(data["status"]==0)
    meanspeed = array(data["spex"]).mean()
    definitions.append((meanspeed,data["definition"],filename))


speeds = [speed for speed,definition,filename in definitions]
definitions.sort(key=lambda x:x[0],reverse=True)
best_definitions = definitions[:50]

bestdir = "best"
if os.path.isdir(bestdir): shutil.rmtree(bestdir)
os.mkdir(bestdir)

print "champions"
for kk,(speed,definition,filename) in enumerate(best_definitions):
    print "%02d %f %s" % (kk,speed,filename)
    shutil.copy(filename,os.path.join(bestdir,"performances.%02d.pck" % kk))

figure()
hist(speeds,bins=100,range=(0,15))
savefig("speed.pdf")

show()

