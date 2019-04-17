#!/usr/bin/env python
# -*- coding: utf-8 -*-

from random import SystemRandom
from string import ascii_uppercase, digits
import os

import sst

BASE_PATH = os.getcwd()
CLOCK = "1MHz"


def get_rand_tmp():
    return ''.join(
        SystemRandom().choice(ascii_uppercase + digits) for _ in range(8)
    )


sst.setProgramOption("stopAtCycle", "50us")

proto_comp = sst.Component("prototype", "proto.prototype")
proto_comp.addParams({
    "clock": CLOCK,
})

galois_lfsr_comp = sst.Component("galois_lfsr", "proto.galois_lfsr")
# override default parameters
galois_lfsr_comp.addParams({
    "proc": os.path.join(BASE_PATH, "galois_lfsr.o"),
    "ipc_port": "/tmp/" + get_rand_tmp(),
    "clock": CLOCK,
})

# galois_lfsr_comp2 = sst.Component("galois_lfsr2", "proto.galois_lfsr")
# # override default parameters
# galois_lfsr_comp2.addParams({
#     "proc": os.path.join(BASE_PATH, "galois_lfsr.o"),
#     "ipc_port": "/tmp/" + get_rand_tmp(),
#     "clock": CLOCK,
# })

fib_lfsr_comp = sst.Component("fib_lfsr", "proto.fib_lfsr")
# override default parameters
fib_lfsr_comp.addParams({
    "proc": os.path.join(BASE_PATH, "fib_lfsr.o"),
    "ipc_port": "/tmp/" + get_rand_tmp(),
    "clock": CLOCK,
})

sst.Link("galois_lfsr_din").connect(
    (galois_lfsr_comp, "galois_lfsr_din", "1ps"),
    (proto_comp, "galois_lfsr_din", "1ps"),
)
sst.Link("galois_lfsr_dout").connect(
    (galois_lfsr_comp, "galois_lfsr_dout", "1ps"),
    (proto_comp, "galois_lfsr_dout", "1ps"),
)

# sst.Link("galois_lfsr_din2").connect(
#     (galois_lfsr_comp2, "galois_lfsr_din", "1ps"),
#     (proto_comp, "galois_lfsr_din", "1ps"),
# )
# sst.Link("galois_lfsr_dout2").connect(
#     (galois_lfsr_comp2, "galois_lfsr_dout", "1ps"),
#     (proto_comp, "galois_lfsr_dout", "1ps"),
# )

sst.Link("fib_lfsr_din").connect(
    (fib_lfsr_comp, "fib_lfsr_din", "1ps"),
    (proto_comp, "fib_lfsr_din", "1ps"),
)
sst.Link("fib_lfsr_dout").connect(
    (fib_lfsr_comp, "fib_lfsr_dout", "1ps"),
    (proto_comp, "fib_lfsr_dout", "1ps"),
)
