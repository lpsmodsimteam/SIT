import os

import sst

# component counter from element counterSST (counterSST.counter) named "counter World"
counterComponent = sst.Component("sst_counter", "sst_counterSST.sst_counter")
sst.setProgramOption("stopAtCycle", "110ns")
# overide default parameters
counterComponent.addParams({
    "sysc_counter": os.path.join(os.getcwd(), "sysc_counter_module.o"),
    "sysc_inverter": os.path.join(os.getcwd(), "sysc_inverter_module.o"),
    "port": "2000",  # socket port number
    "num_procs": "2"  # socket port number
})
