import os

import sst

BASE_PATH = os.getcwd()

sst.setProgramOption("stopAtCycle", "80ns")

galois_lfsr_comp = sst.Component("sst_galois_lfsr", "proto1.sst_galois_lfsr")
# overide default parameters
galois_lfsr_comp.addParams({
    "proc": os.path.join(BASE_PATH, "galois_lfsr.o"),
    "port": "ipc:///tmp/galois",
})

proto_comp = sst.Component("proto1", "proto1.sst_dev")
# # overide default parameters
# proto_comp.addParams({
#     "proc": os.path.join(BASE_PATH, "galois_lfsr.o"),
#     "port": "ipc:///tmp/galois",
# })
#
# fib_lfsr_comp = sst.Component("sst_fib_lfsr", "proto2.sst_fib_lfsr")
# # overide default parameters
# fib_lfsr_comp.addParams({
#     "proc": os.path.join(BASE_PATH, "fib_lfsr.o"),
#     "port": "ipc:///tmp/fib",
# })

# Connect the carWash and carGenerator components
sst.Link("Road").connect(
    (galois_lfsr_comp, "port", "1ps"),
    (proto_comp, "port", "1ps"),
)
