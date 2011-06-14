#!/usr/bin/env python2
# coding: utf-8

import cPickle
import math
import sys
import random

filename  = sys.argv[1]

data = {}
data["motorRadius"] = 1.5
data["mainLength"] = 10.
data["mainHeight"] = 2.
data["upperExtension"] = random.randrange(.5,5,int=float)
data["legWidth"] = random.randrange(1,4,int=float)
data["legHeight"] = random.randrange(2,8,int=float)
data["legAngle"] = random.random(-5,45,int=float)/180.*math.pi
data["footHeight"] = random.randrange(2,8,int=float)
data["legNumber"] = random.randrange(2,6)

cPickle.dump(data,file(filename,"wb"))
