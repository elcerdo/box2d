#!/usr/bin/env python2
# coding: utf-8

import os 
import sys 
import cPickle as pickle

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

definitions.sort(key=lambda x:x[0],reverse=True)

print "champions"
print '\n'.join("%f %s" % (speed,filename) for speed,definition,filename in definitions[:30])


