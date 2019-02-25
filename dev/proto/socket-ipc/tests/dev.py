import os

import sst

# component counter from element counterSST (counterSST.counter) named "counter World"
counterComponent = sst.Component("sst_dev", "sst_devSST.sst_dev")
sst.setProgramOption("stopAtCycle", "80ns")
# overide default parameters
counterComponent.addParams({
    "sysc_counter": os.path.join(os.getcwd(), "sysc_counter_module.o"),
    "sysc_inverter": os.path.join(os.getcwd(), "sysc_inverter_module.o"),
    "sysc_sr": os.path.join(os.getcwd(), "sysc_sr_module.o"),
    "num_procs": "1"  # number of child modules
})
