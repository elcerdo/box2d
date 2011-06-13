#!/usr/bin/env python2
# coding: utf-8

import cPickle
import math

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

cPickle.dump(data,file("robot.pck","wb"))
