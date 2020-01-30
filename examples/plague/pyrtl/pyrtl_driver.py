#!/usr/bin/env python
# -*- coding: utf-8 -*-


from mutation import *

sim.step({
    "chance": 3,
    "gene": 2,
})
print(sim.inspect(out))
sim.step({
    "chance": 9,
    "gene": 9,
})
print(sim.inspect(out))
print(sim.inspect(out))
sim.step({
    "chance": 9,
    "gene": 9,
})
print(sim.inspect(out))
print(sim.inspect(out))
print(sim.inspect(out))
print(sim.inspect(out))
print(sim.inspect(out))
sim.step({
    "chance": 9,
    "gene": 9,
})
print(sim.inspect(out))
