from random import SystemRandom
from string import ascii_uppercase, digits
import os

import sst

BASE_PATH = os.getcwd()
CLOCK = "500MHz"
sst.setProgramOption("stopAtCycle", "80ns")

proto_comp = sst.Component("sst_dev", "proto.sst_dev")
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

sst.Link("galois_reset").connect(
    (galois_lfsr_comp, "galois_reset", "1ps"),
    (proto_comp, "galois_reset", "1ps"),
)
sst.Link("galois_clock").connect(
    (galois_lfsr_comp, "galois_clock", "1ps"),
    (proto_comp, "galois_clock", "1ps"),
)
sst.Link("galois_data_out").connect(
    (galois_lfsr_comp, "galois_data_out", "1ps"),
    (proto_comp, "galois_data_out", "1ps"),
)

sst.Link("fib_reset").connect(
    (fib_lfsr_comp, "fib_reset", "1ps"),
    (proto_comp, "fib_reset", "1ps"),
)
sst.Link("fib_clock").connect(
    (fib_lfsr_comp, "fib_clock", "1ps"),
    (proto_comp, "fib_clock", "1ps"),
)
sst.Link("fib_data_out").connect(
    (fib_lfsr_comp, "fib_data_out", "1ps"),
    (proto_comp, "fib_data_out", "1ps"),
)
