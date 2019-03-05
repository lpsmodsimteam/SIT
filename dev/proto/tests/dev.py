import os

import sst

BASE_PATH = os.getcwd()

sst.setProgramOption("stopAtCycle", "80ns")

proto_comp = sst.Component("sst_dev", "proto.sst_dev")

galois_lfsr_comp = sst.Component("sst_galois_lfsr", "proto.sst_galois_lfsr")
# overide default parameters
galois_lfsr_comp.addParams({
    "proc": os.path.join(BASE_PATH, "galois_lfsr.o"),
    "port": "ipc:///tmp/galois",
})

fib_lfsr_comp = sst.Component("sst_fib_lfsr", "proto.sst_fib_lfsr")
# overide default parameters
fib_lfsr_comp.addParams({
    "proc": os.path.join(BASE_PATH, "fib_lfsr.o"),
    "port": "ipc:///tmp/fib",
})

sst.Link("Road").connect(
    (galois_lfsr_comp, "port", "1ps"),
    (proto_comp, "port", "1ps"),
)
sst.Link("Road2").connect(
    (fib_lfsr_comp, "port", "1ps"),
    (proto_comp, "port", "1ps"),
)
