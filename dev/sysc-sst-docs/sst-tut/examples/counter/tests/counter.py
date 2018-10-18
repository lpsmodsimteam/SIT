import sst

# component counter from element counterSST (counterSST.counter) named "counter World"
counterComponent = sst.Component("counter", "counterSST.counter")
sst.setProgramOption("stopAtCycle", "1us")
# overide default parameters
counterComponent.addParams({
    # "printFrequency": "5",  # print every 5 ticks
    # "repeats": "15"  # run for 15 ticks
})
