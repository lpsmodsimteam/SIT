import os

import sst

BASE_PATH = os.getcwd()

sst.setProgramOption("stopAtCycle", "80ns")

proto_comp = sst.Component("sst_dev", "proto.sst_dev")
proto_comp.addParams({
    "clock": "500MHz",
})

galois_lfsr_comp = sst.Component("sst_galois_lfsr", "proto.sst_galois_lfsr")
# overide default parameters
galois_lfsr_comp.addParams({
    "proc": os.path.join(BASE_PATH, "galois_lfsr.o"),
    "ipc_port": "ipc:///tmp/galois",
    "clock": "500MHz",
})

fib_lfsr_comp = sst.Component("sst_fib_lfsr", "proto.sst_fib_lfsr")
# overide default parameters
fib_lfsr_comp.addParams({
    "proc": os.path.join(BASE_PATH, "fib_lfsr.o"),
    "ipc_port": "ipc:///tmp/fib",
    "clock": "500MHz",
})

sst.Link("road0").connect(
    (galois_lfsr_comp, "link_galois", "1ps"),
    (proto_comp, "link_galois", "1ps"),
)
sst.Link("road1").connect(
    (fib_lfsr_comp, "link_fib", "1ps"),
    (proto_comp, "link_fib", "1ps"),
)
