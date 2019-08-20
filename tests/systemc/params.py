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


def unit_test(ipc, component):

    if ipc == "sock":
        port_prefix = "/tmp/"
    elif ipc == "zmq":
        port_prefix = "ipc:///tmp/"

    sst.setProgramOption("stopAtCycle", "50us")

    proto_comp = sst.Component(component, "proto." + component)
    proto_comp.addParams({
        "clock": CLOCK,
    })

    galois_lfsr_comp = sst.Component("galois_lfsr", "proto.galois_lfsr")
    # override default parameters
    galois_lfsr_comp.addParams({
        "proc": os.path.join(BASE_PATH, "galois_lfsr.o"),
        "ipc_port": port_prefix + get_rand_tmp(),
        "clock": CLOCK,
    })

    fib_lfsr_comp = sst.Component("fib_lfsr", "proto.fib_lfsr")
    # override default parameters
    fib_lfsr_comp.addParams({
        "proc": os.path.join(BASE_PATH, "fib_lfsr.o"),
        "ipc_port": port_prefix + get_rand_tmp(),
        "clock": CLOCK,
    })

    sst.Link("galois_lfsr_din").connect(
        (galois_lfsr_comp, "galois_lfsr_din", LINK_SPEED),
        (proto_comp, "galois_lfsr_din", LINK_SPEED),
    )
    sst.Link("galois_lfsr_dout").connect(
        (galois_lfsr_comp, "galois_lfsr_dout", LINK_SPEED),
        (proto_comp, "galois_lfsr_dout", LINK_SPEED),
    )

    sst.Link("fib_lfsr_din").connect(
        (fib_lfsr_comp, "fib_lfsr_din", LINK_SPEED),
        (proto_comp, "fib_lfsr_din", LINK_SPEED),
    )
    sst.Link("fib_lfsr_dout").connect(
        (fib_lfsr_comp, "fib_lfsr_dout", LINK_SPEED),
        (proto_comp, "fib_lfsr_dout", LINK_SPEED),
    )
