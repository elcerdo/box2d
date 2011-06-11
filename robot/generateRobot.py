#!/usr/bin/env python
# coding: utf-8

import cPickle
import math
import sys

filename  = sys.argv[1]

data = {}
data["motorRadius"] = 1.5
data["mainLength"] = 10.
data["mainHeight"] = 2.
data["upperExtension"] = 3.
data["legWidth"] = 3.
data["legHeight"] = 5.
data["legAngle"] = 20/180.*math.pi
data["footHeight"] = 5.
data["legNumber"] = 5

cPickle.dump(data,file(filename,"wb"))
