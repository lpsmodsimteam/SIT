import os

import sst

# component counter from element counterSST (counterSST.counter) named "counter World"
counterComponent = sst.Component("sst_dev", "sst_devSST.sst_dev")
sst.setProgramOption("stopAtCycle", "80ns")
# overide default parameters
counterComponent.addParams({
    "lrs": os.path.join(os.getcwd(), "lrs.o"),
    "galois_lfsr": os.path.join(os.getcwd(), "galois_lfsr.o"),
    "num_procs": "2"  # number of child modules
})
