import os

import sst

BASE_PATH = os.getcwd()

sst.setProgramOption("stopAtCycle", "80ns")

galois_lfsr_comp = sst.Component("socktest", "socktest.socktest")
# overide default parameters
galois_lfsr_comp.addParams({
    "proc": os.path.join(BASE_PATH, "galois_lfsr.o"),
    "ipc_port": "/tmp/galois3",
    "clock": "500MHz",
})
