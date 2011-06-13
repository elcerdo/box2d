#!/usr/bin/env python2
# coding: utf-8

from pylab import *
import cPickle as pickle
import sys

filename = sys.argv[1]
data = pickle.load(file(filename))
#print "\n".join("%s = %f" % item for item in data["definition"].items())

position = array([data["posx"],data["posy"]]).transpose()
speed = array([data["spex"],data["spey"]]).transpose()
bodyangle = array(data["bodyangle"])*pi/180.
engineangle = array(data["engineangle"])*pi/180.
time = array(data["time"])

figure()
title("position")
plot(position[:,0],position[:,1])
xlabel("x [m]")
ylabel("y [m]")

figure()
title("xspeed")
plot(time,speed[:,0])
xlabel("t [s]")
ylabel("vx [m/s]")

figure()
title("yspeed")
plot(time,speed[:,1])
xlabel("t [s]")
ylabel("vy [m/s]")

figure()
polar(engineangle,speed[:,0])

show()


