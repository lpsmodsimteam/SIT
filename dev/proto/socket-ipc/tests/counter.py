import sst

# component counter from element counterSST (counterSST.counter) named "counter World"
counterComponent = sst.Component("sst_counter", "sst_counterSST.sst_counter")
sst.setProgramOption("stopAtCycle", "600ns")
# overide default parameters
counterComponent.addParams({
    # "printFrequency": "5",  # print every 5 ticks
    # "repeats": "15"  # run for 15 ticks
})
