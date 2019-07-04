#!/usr/bin/env python
# -*- coding: utf-8 -*-

import pyrtl

# Inputs and outputs of the module
load = pyrtl.Input(1, "LOAD")  # using load as reset
start_green = pyrtl.Input(1, "STARTGREEN")
green_time = pyrtl.Input(8, "GREENTIME")
yellow_time = pyrtl.Input(8, "YELLOWTIME")
red_time = pyrtl.Input(8, "REDTIME")
out = pyrtl.Output(2, "out")

# Register the times for each color when load signal is 1
gt = pyrtl.Register(8, "gt")
yt = pyrtl.Register(8, "yt")
rt = pyrtl.Register(8, "rt")
with pyrtl.conditional_assignment:
    with load:
        gt.next |= green_time - 1
        yt.next |= yellow_time - 1
        rt.next |= red_time - 1

# Registers for state and a counter
counter = pyrtl.Register(8, "counter")
state = pyrtl.Register(2, "state")
# enumerate a set of constant to serve as our states
GREEN, YELLOW, RED = [pyrtl.Const(x, bitwidth=2) for x in range(3)]

# PyRTL state machine
with pyrtl.conditional_assignment:
    # Using load as the reset signal
    with load:
        counter.next |= 0
        # 2 entry points into the state machine
        with start_green:
            state.next |= GREEN
        with pyrtl.otherwise:
            state.next |= RED

    # Main state machine, count until the appropriate time limit for each state
    with pyrtl.otherwise:
        with state == RED:
            with counter == rt:
                state.next |= GREEN
                counter.next |= 0
            with pyrtl.otherwise:
                counter.next |= counter + 1

        with state == GREEN:
            with counter == gt:
                state.next |= YELLOW
                counter.next |= 0
            with pyrtl.otherwise:
                counter.next |= counter + 1

        with state == YELLOW:
            with counter == yt:
                state.next |= RED
                counter.next |= 0
            with pyrtl.otherwise:
                counter.next |= counter + 1
# Output the state
out <<= state

# Setup the simulation
sim_trace = pyrtl.SimulationTrace([load, start_green, counter, out])
sim = pyrtl.FastSimulation(tracer=sim_trace)
