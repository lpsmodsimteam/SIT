import os

import sst

BASE_PATH = os.getcwd()

sst.setProgramOption("stopAtCycle", "80ns")

galois_lfsr_comp = sst.Component("sst_galois_lfsr", "sst_galois_lfsrSST.sst_galois_lfsr")
# overide default parameters
galois_lfsr_comp.addParams({
    "proc": os.path.join(BASE_PATH, "galois_lfsr.o"),
    "port": "ipc:///tmp/galois",
})

fib_lfsr_comp = sst.Component("sst_fib_lfsr", "sst_fib_lfsrSST.sst_fib_lfsr")
# overide default parameters
fib_lfsr_comp.addParams({
    "proc": os.path.join(BASE_PATH, "fib_lfsr.o"),
    "port": "ipc:///tmp/fib",
})
