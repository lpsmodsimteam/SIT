#!/usr/bin/env python
# -*- coding: utf-8 -*-

from random import SystemRandom
from string import ascii_uppercase, digits
import os

import sst

BASE_PATH = os.getcwd()
CLOCK = "1Hz"

GREEN0TIME = 30
GREEN1TIME = 60
YELLOWTIME = 3


def get_rand_tmp():
    return ''.join(
        SystemRandom().choice(ascii_uppercase + digits) for _ in range(8)
    )


light_comp0 = sst.Component("Traffic Light 0", "intersection.traffic_light")
light_comp0.addParams({
    "clock": CLOCK,
    "GREENTIME": GREEN0TIME,
    "YELLOWTIME": YELLOWTIME,
    "REDTIME": GREEN1TIME + YELLOWTIME,
    "STARTGREEN": 0
})
light_comp1 = sst.Component("Traffic Light 1", "intersection.traffic_light")
light_comp1.addParams({
    "clock": CLOCK,
    "GREENTIME": GREEN1TIME,
    "YELLOWTIME": YELLOWTIME,
    "REDTIME": GREEN0TIME + YELLOWTIME,
    "STARTGREEN": 1
})

stoplight_comp0 = sst.Component(
    "Stoplight Blackbox 0", "intersection.stoplight")
stoplight_comp0.addParams({
    "proc": os.path.join(BASE_PATH, "stoplight.o"),
    "ipc_port": "/tmp/" + get_rand_tmp(),
    "clock": CLOCK,
})

stoplight_comp1 = sst.Component(
    "Stoplight Blackbox 1", "intersection.stoplight")
stoplight_comp1.addParams({
    "proc": os.path.join(BASE_PATH, "stoplight.o"),
    "ipc_port": "/tmp/" + get_rand_tmp(),
    "clock": CLOCK,
})

car_generator0 = sst.Component("Car Generator 0", "intersection.car_generator")
car_generator0.addParams({
    "delay": "3s",
    "randomseed": 151515
})

car_generator1 = sst.Component("Car Generator 1", "intersection.car_generator")
car_generator1.addParams({
    "delay": "5s",
    "randomseed": 239478
})

intersection = sst.Component("Intersection", "intersection.intersection")
intersection.addParams({
    "clock": CLOCK,
    "simDuration": 86400
})

# connect the blackbox interface components
sst.Link("stoplight_din0").connect(
    (stoplight_comp0, "stoplight_din", "1ps"),
    (light_comp0, "stoplight_din", "1ps"),
)
sst.Link("stoplight_dout0").connect(
    (stoplight_comp0, "stoplight_dout", "1ps"),
    (light_comp0, "stoplight_dout", "1ps"),
)

sst.Link("stoplight_din1").connect(
    (stoplight_comp1, "stoplight_din", "1ps"),
    (light_comp1, "stoplight_din", "1ps"),
)
sst.Link("stoplight_dout1").connect(
    (stoplight_comp1, "stoplight_dout", "1ps"),
    (light_comp1, "stoplight_dout", "1ps"),
)

# connect the subcompononents
sst.Link("light0").connect(
    (intersection, "light0", "1ps"),
    (light_comp0, "port", "1ps")
)
sst.Link("light1").connect(
    (intersection, "light1", "1ps"),
    (light_comp1, "port", "1ps")
)
sst.Link("cars0").connect(
    (intersection, "cars0", "1ps"),
    (car_generator0, "port", "1ps")
)
sst.Link("cars1").connect(
    (intersection, "cars1", "1ps"),
    (car_generator1, "port", "1ps")
)
