import os

import sst

# component counter from element counterSST (counterSST.counter) named "counter World"
counterComponent = sst.Component("sst_dev", "sst_devSST.sst_dev")
sst.setProgramOption("stopAtCycle", "110ns")
# overide default parameters
counterComponent.addParams({
    "counter": os.path.join(os.getcwd(), "counter.o"),
    # "inverter": os.path.join(os.getcwd(), "inverter.o"),
    "lrs": os.path.join(os.getcwd(), "lrs.o"),
    "galois_lfsr": os.path.join(os.getcwd(), "galois_lfsr.o"),
    "num_procs": 3  # number of child modules
})
