from random import SystemRandom
from string import ascii_uppercase, digits
import subprocess

import sst

light0green = 30
light1green = 60
yellow = 3

def get_rand_tmp():
    return ''.join(SystemRandom().choice(ascii_uppercase + digits) for _ in range(8))

input0 = "/tmp/" + get_rand_tmp()
output0 = "/tmp/" + get_rand_tmp()
subprocess.Popen(['python', 'stopLight.py', input0, output0])
light0 = sst.Component("Traffic Light 0", "intersection.traffic_light")
light0.addParams({
    "clock": "1Hz",
    "greenTime": light0green,
    "yellowTime": yellow,
    "redTime": light1green + yellow,
    "startGreen": 0,
    "inputPipe": output0,
    "outputPipe": input0
})

input1 = "/tmp/" + get_rand_tmp()
output1 = "/tmp/" + get_rand_tmp()
subprocess.Popen(['python', 'stopLight.py', input1, output1])
light1 = sst.Component("Traffic Light 1", "intersection.traffic_light")
light1.addParams({
    "clock": "1Hz",
    "greenTime": light1green,
    "yellowTime": yellow,
    "redTime": light0green + yellow,
    "startGreen": 1,
    "inputPipe": output1,
    "outputPipe": input1
})

carGenerator0 = sst.Component("Car Generator 0", "intersection.car_generator")
carGenerator0.addParams({
    "delay": "3s",
    "randomseed": 151515
})

carGenerator1 = sst.Component("Car Generator 1", "intersection.car_generator")
carGenerator1.addParams({
    "delay": "5s",
    "randomseed": 239478
})

intersection = sst.Component("Intersection", "intersection.intersection")
intersection.addParams({
    "clock": "1Hz",
    "simDuration": 86400
})

sst.Link("light0").connect((intersection, "light0", "1ps"), (light0, "port", "1ps"))
sst.Link("light1").connect((intersection, "light1", "1ps"), (light1, "port", "1ps"))
sst.Link("cars0").connect((intersection, "cars0", "1ps"), (carGenerator0, "port", "1ps"))
sst.Link("cars1").connect((intersection, "cars1", "1ps"), (carGenerator1, "port", "1ps"))
