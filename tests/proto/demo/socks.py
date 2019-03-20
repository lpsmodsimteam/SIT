#!/usr/bin/env python
# -*- coding: utf-8 -*-

from random import SystemRandom
from string import ascii_uppercase, digits
import os

import sst

BASE_PATH = os.getcwd()
CLOCK = "500MHz"
sst.setProgramOption("stopAtCycle", "80ns")

proto_comp = sst.Component("prototype", "proto.prototype")
proto_comp.addParams({
    "clock": CLOCK,
})

galois_lfsr_comp = sst.Component("galois_lfsr", "proto.galois_lfsr")
# overide default parameters
galois_lfsr_comp.addParams({
    "proc": os.path.join(BASE_PATH, "galois_lfsr.o"),
    "ipc_port": "/tmp/" + ''.join(SystemRandom().choice(ascii_uppercase + digits) for _ in range(8)),
    "clock": CLOCK,
})

fib_lfsr_comp = sst.Component("fib_lfsr", "proto.fib_lfsr")
# overide default parameters
fib_lfsr_comp.addParams({
    "proc": os.path.join(BASE_PATH, "fib_lfsr.o"),
    "ipc_port": "/tmp/" + ''.join(SystemRandom().choice(ascii_uppercase + digits) for _ in range(8)),
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

sst.Link("fib_lfsr_din").connect(
    (fib_lfsr_comp, "fib_lfsr_din", "1ps"),
    (proto_comp, "fib_lfsr_din", "1ps"),
)
sst.Link("fib_lfsr_dout").connect(
    (fib_lfsr_comp, "fib_lfsr_dout", "1ps"),
    (proto_comp, "fib_lfsr_dout", "1ps"),
)
