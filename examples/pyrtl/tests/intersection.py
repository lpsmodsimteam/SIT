import sst
import subprocess

light0green = 30
light1green = 60
yellow = 3

subprocess.Popen('python stopLight.py /tmp/input0 /tmp/output0'.split(' '))
light0 = sst.Component("Traffic Light 0", "intersection.trafficLight")
light0.addParams({
	"clock" : "1Hz",
	"greenTime" : light0green,
	"yellowTime" : yellow,
	"redTime" : light1green + yellow,
	"startGreen" : "0",
	"inputPipe" : "/tmp/output0",
	"outputPipe" : "/tmp/input0"
	})


subprocess.Popen('python stopLight.py /tmp/input1 /tmp/output1'.split(' '))
light1 = sst.Component("Traffic Light 1", "intersection.trafficLight")
light1.addParams({
	"clock" : "1Hz",
	"greenTime" : light1green,
	"yellowTime" : yellow,
	"redTime" : light0green + yellow,
	"startGreen" : "1",
	"inputPipe" : "/tmp/output1",
	"outputPipe" : "/tmp/input1"
	})


carGenerator0 = sst.Component("Car Generator 0", "intersection.carGenerator")
carGenerator0.addParams({
	"delay" : "3s",
	"randomseed" : "151515"
	})

carGenerator1 = sst.Component("Car Generator 1", "intersection.carGenerator")
carGenerator1.addParams({
	"delay" : "5s",
	"randomseed" : "239478"
	})

intersection = sst.Component("Intersection", "intersection.intersection")
intersection.addParams({
	"clock" : "1Hz",
	"simDuration" : "86400"
	})


sst.Link("light0").connect( (intersection, "light0", "1ps"), (light0, "port", "1ps") )
sst.Link("light1").connect( (intersection, "light1", "1ps"), (light1, "port", "1ps") )
sst.Link("cars0").connect( (intersection, "cars0", "1ps"), (carGenerator0, "port", "1ps") )
sst.Link("cars1").connect( (intersection, "cars1", "1ps"), (carGenerator1, "port", "1ps") )
