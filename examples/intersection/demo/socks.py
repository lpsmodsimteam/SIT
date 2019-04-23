#!/usr/bin/env python
# -*- coding: utf-8 -*-

from random import SystemRandom
from string import ascii_uppercase, digits
import os

import sst

BASE_PATH = os.getcwd()
CLOCK = "1Hz"
LINK_DELAY = "1ps"

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
    "STARTGREEN": 0,
    "proc": os.path.join(BASE_PATH, "traffic_light_fsm.o"),
    "ipc_port": "/tmp/" + get_rand_tmp(),
})
light_comp1 = sst.Component("Traffic Light 1", "intersection.traffic_light")
light_comp1.addParams({
    "clock": CLOCK,
    "GREENTIME": GREEN1TIME,
    "YELLOWTIME": YELLOWTIME,
    "REDTIME": GREEN0TIME + YELLOWTIME,
    "STARTGREEN": 1,
    "proc": os.path.join(BASE_PATH, "traffic_light_fsm.o"),
    "ipc_port": "/tmp/" + get_rand_tmp(),
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
    "sim_duration": 86400
})


# connect the subcomponents
sst.Link("light0").connect(
    (intersection, "light0", LINK_DELAY),
    (light_comp0, "light_state", LINK_DELAY)
)
sst.Link("light1").connect(
    (intersection, "light1", LINK_DELAY),
    (light_comp1, "light_state", LINK_DELAY)
)
sst.Link("cars0").connect(
    (intersection, "cars0", LINK_DELAY),
    (car_generator0, "is_car", LINK_DELAY)
)
sst.Link("cars1").connect(
    (intersection, "cars1", LINK_DELAY),
    (car_generator1, "is_car", LINK_DELAY)
)
