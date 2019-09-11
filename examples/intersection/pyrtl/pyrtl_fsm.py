#!/usr/bin/env python
# -*- coding: utf-8 -*-

import pyrtl

# Inputs and outputs of the module
load = pyrtl.Input(1, "load")  # using load as reset
start_green = pyrtl.Input(1, "start_green")
green_time = pyrtl.Input(6, "green_time")
yellow_time = pyrtl.Input(2, "yellow_time")
red_time = pyrtl.Input(6, "red_time")
state = pyrtl.Output(2, "state")

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
state_reg = pyrtl.Register(2, "state_reg")
# enumerate a set of constant to serve as our states
GREEN, YELLOW, RED = [pyrtl.Const(x, bitwidth=2) for x in range(3)]

# PyRTL state machine
with pyrtl.conditional_assignment:
    # Using load as the reset signal
    with load:
        counter.next |= 0
        # 2 entry points into the state machine
        with start_green:
            state_reg.next |= GREEN
        with pyrtl.otherwise:
            state_reg.next |= RED

    # Main state machine, count until the appropriate time limit for each state
    with pyrtl.otherwise:
        with state_reg == RED:
            with counter == rt:
                state_reg.next |= GREEN
                counter.next |= 0
            with pyrtl.otherwise:
                counter.next |= counter + 1

        with state_reg == GREEN:
            with counter == gt:
                state_reg.next |= YELLOW
                counter.next |= 0
            with pyrtl.otherwise:
                counter.next |= counter + 1

        with state_reg == YELLOW:
            with counter == yt:
                state_reg.next |= RED
                counter.next |= 0
            with pyrtl.otherwise:
                counter.next |= counter + 1
# Output the state
state <<= state_reg

# Setup the simulation
sim_trace = pyrtl.SimulationTrace([load, start_green, counter, state])
sim = pyrtl.FastSimulation(tracer=sim_trace)
