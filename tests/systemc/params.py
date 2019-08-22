#!/usr/bin/env python
# -*- coding: utf-8 -*-

from random import SystemRandom
from string import ascii_uppercase, digits
import os

import sst

BASE_PATH = os.getcwd()
CLOCK = "1MHz"
LINK_SPEED = "1ps"


def get_rand_tmp():
    return ''.join(
        SystemRandom().choice(ascii_uppercase + digits) for _ in range(8)
    )


def test_setup(ipc, component):

    if ipc == "sock":
        port_prefix = "/tmp/"
    elif ipc == "zmq":
        port_prefix = "ipc:///tmp/"

    sst.setProgramOption("stopAtCycle", "25us")

    systemc_comp = sst.Component(component, "systemc." + component)
    systemc_comp.addParams({
        "clock": CLOCK,
    })

    ram_comp = sst.Component("ram", "systemc.ram")
    # override default parameters
    ram_comp.addParams({
        "proc": os.path.join(BASE_PATH, "ram.o"),
        "ipc_port": port_prefix + get_rand_tmp(),
        "clock": CLOCK,
    })

    sst.Link("ram_din").connect(
        (ram_comp, "ram_din", LINK_SPEED),
        (systemc_comp, "ram_din", LINK_SPEED),
    )
    sst.Link("ram_dout").connect(
        (ram_comp, "ram_dout", LINK_SPEED),
        (systemc_comp, "ram_dout", LINK_SPEED),
    )
