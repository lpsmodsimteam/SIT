import os

import sst

# component counter from element counterSST (counterSST.counter) named "counter World"
counterComponent = sst.Component("sst_dev", "sst_devSST.sst_dev")
sst.setProgramOption("stopAtCycle", "80ns")
BASE_PATH = os.getcwd()
# overide default parameters
counterComponent.addParams({
    "lrs": os.path.join(BASE_PATH, "lrs.o"),
    "galois_lfsr": os.path.join(BASE_PATH, "galois_lfsr.o"),
    "num_procs": 3  # number of child modules
})
