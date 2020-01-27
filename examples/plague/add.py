#!/usr/bin/env python
# -*- coding: utf-8 -*-

import random

import pyrtl

op1 = pyrtl.Input(33, "op1")
op2 = pyrtl.Input(33, "op2")
out = pyrtl.Output(33, "out")

out <<= (op1 / op2) + ((op1 % op2) != 0)

# Setup the simulation
sim_trace = pyrtl.SimulationTrace([op1, op2, out])
sim = pyrtl.FastSimulation(tracer=sim_trace)
