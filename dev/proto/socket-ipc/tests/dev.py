import os

import sst

dev_comp = sst.Component("sst_dev", "sst_devSST.sst_dev")
sst.setProgramOption("stopAtCycle", "90ns")
BASE_PATH = os.getcwd()
# overide default parameters
dev_comp.addParams({
    "mods": [
        os.path.join(BASE_PATH, "lrs.o"),
        os.path.join(BASE_PATH, "galois_lfsr.o"),
    ],
    "num_procs": 2  # number of child modules
})
