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
    return "/tmp/" + ''.join(
        SystemRandom().choice(ascii_uppercase + digits) for _ in range(8)
    )


# SystemC
systemc_main = sst.Component(
    "Traffic Light (SystemC)", "intersection.traffic_light_systemc")
systemc_main.addParams({
    "CLOCK": CLOCK,
    "GREENTIME": GREEN0TIME,
    "YELLOWTIME": YELLOWTIME,
    "REDTIME": GREEN1TIME + YELLOWTIME,
    "STARTGREEN": 0
})

systemc_comp = sst.Component(
    "SystemC Component", "intersection.systemc_fsm")
systemc_comp.addParams({
    "clock": CLOCK,
    "proc": os.path.join(BASE_PATH, "systemc_fsm.o"),
    "ipc_port": get_rand_tmp(),
})

# PyRTL
pyrtl_main = sst.Component(
    "Traffic Light (PyRTL)", "intersection.traffic_light_pyrtl")
pyrtl_main.addParams({
    "CLOCK": CLOCK,
    "GREENTIME": GREEN1TIME,
    "YELLOWTIME": YELLOWTIME,
    "REDTIME": GREEN0TIME + YELLOWTIME,
    "STARTGREEN": 1
})
pyrtl_comp = sst.Component(
    "PyRTL Component", "intersection.pyrtl_fsm")
pyrtl_comp.addParams({
    "clock": CLOCK,
    "proc": os.path.join(BASE_PATH, "../pyrtl/blackboxes/pyrtl_fsm_driver.py"),
    "ipc_port": get_rand_tmp(),
})

car_generator0 = sst.Component("Car Generator 0", "intersection.car_generator")
car_generator0.addParams({
    "DELAY": "3s",
    "RANDOMSEED": 151515
})

car_generator1 = sst.Component("Car Generator 1", "intersection.car_generator")
car_generator1.addParams({
    "DELAY": "5s",
    "RANDOMSEED": 239478
})

intersection = sst.Component("Intersection", "intersection.intersection")
intersection.addParams({
    "CLOCK": CLOCK,
})


# connect the subcomponents
sst.Link("sc_din").connect(
    (systemc_comp, "systemc_fsm_din", LINK_DELAY),
    (systemc_main, "sc_din", LINK_DELAY)
)
sst.Link("sc_dout").connect(
    (systemc_comp, "systemc_fsm_dout", LINK_DELAY),
    (systemc_main, "sc_dout", LINK_DELAY)
)
sst.Link("light0").connect(
    (intersection, "light0", LINK_DELAY),
    (systemc_main, "light_state", LINK_DELAY)
)

sst.Link("py_din").connect(
    (pyrtl_comp, "pyrtl_fsm_din", LINK_DELAY),
    (pyrtl_main, "py_din", LINK_DELAY)
)
sst.Link("py_dout").connect(
    (pyrtl_comp, "pyrtl_fsm_dout", LINK_DELAY),
    (pyrtl_main, "py_dout", LINK_DELAY)
)
sst.Link("light1").connect(
    (intersection, "light1", LINK_DELAY),
    (pyrtl_main, "light_state", LINK_DELAY)
)

sst.Link("cars0").connect(
    (intersection, "cars0", LINK_DELAY),
    (car_generator0, "is_car", LINK_DELAY)
)
sst.Link("cars1").connect(
    (intersection, "cars1", LINK_DELAY),
    (car_generator1, "is_car", LINK_DELAY)
)

sst.setProgramOption("stopAtCycle", "86401s")
