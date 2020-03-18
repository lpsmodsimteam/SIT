#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
from random import SystemRandom
from string import ascii_uppercase, digits

import sst

BASE_PATH = os.getcwd()
CLOCK = "1MHz"
LINK_SPEED = "1ps"


def get_rand_tmp():
    return ''.join(
        SystemRandom().choice(ascii_uppercase + digits) for _ in range(8)
    )


def setup(hdl, ipc, comp):
    if ipc == "sock":
        port_prefix = "/tmp/"
    elif ipc == "zmq":
        port_prefix = "ipc:///tmp/"
    else:
        raise NotImplementedError()

    sst.setProgramOption("stopAtCycle", "25us")

    main_comp = sst.Component(comp, "tests." + comp)
    main_comp.addParams({
        "clock": CLOCK,
    })

    ram_comp = sst.Component("ram", "tests.ram")

    if hdl == "systemc":
        proc = os.path.join(BASE_PATH, "ram.o")
    elif hdl == "pyrtl":
        proc = os.path.join(os.path.dirname(BASE_PATH), ipc, "blackboxes", "ram_driver.py")
    elif hdl == "chisel":
        proc = "test:runMain ram.ramMain"
    elif hdl == "verilog":
        proc = "PLUSARGS=+ipc_proc="
    else:
        raise NotImplementedError()
    ram_comp.addParams({
        "proc": proc,
        "ipc_port": port_prefix + get_rand_tmp(),
        "clock": CLOCK,
    })

    sst.Link("ram_din").connect(
        (ram_comp, "ram_din", LINK_SPEED),
        (main_comp, "ram_din", LINK_SPEED),
    )
    sst.Link("ram_dout").connect(
        (ram_comp, "ram_dout", LINK_SPEED),
        (main_comp, "ram_dout", LINK_SPEED),
    )
