#!/usr/bin/env python
# -*- coding: utf-8 -*-

import random

import pyrtl

limit = pyrtl.Input(10, "limit")
rng = pyrtl.Output(10, "rng")

# rng_reg = pyrtl.Register(10, "rng_reg")
# with pyrtl.conditional_assignment:
cnst = [pyrtl.Const(limit, bitwidth=10)]
print(limit, cnst)

rng <<= limit - 1
# print(limit, rng_reg)
# rng <<= random.randint(1, limit)

# Setup the simulation
sim_trace = pyrtl.SimulationTrace([limit, rng])
sim = pyrtl.FastSimulation(tracer=sim_trace)
