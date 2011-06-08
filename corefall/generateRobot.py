#!/usr/bin/env python
# coding: utf-8

import cPickle
import math

data = {}
data["motorRadius"] = 1.5
data["mainLength"] = 15.
data["mainHeight"] = 2.
data["upperExtension"] = 3.
data["legWidth"] = 3.
data["legHeight"] = 5.
data["legAngle"] = 10/180.*math.pi
data["footHeight"] = 7.
data["legNumber"] = 3

cPickle.dump(data,file("robot.pck","wb"))
