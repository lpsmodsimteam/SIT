#!/usr/bin/env python
# -*- coding: utf-8 -*-

import pyrtl
pyrtl.set_debug_mode(debug=True)

DATA_WIDTH = 8
ADDR_WIDTH = 8
RAM_DEPTH = 1 << 8

ADDRESS = pyrtl.Input(ADDR_WIDTH, "ADDRESS")
CS = pyrtl.Input(1, "CS")
WE = pyrtl.Input(1, "WE")
OE = pyrtl.Input(1, "OE")
DATA_IN = pyrtl.Input(DATA_WIDTH, "DATA_IN")
DATA_OUT = pyrtl.Output(DATA_WIDTH, "DATA_OUT")

mem = pyrtl.memory.MemBlock(RAM_DEPTH, ADDR_WIDTH,
                            name="mem", asynchronous=True)
mem[ADDRESS] <<= pyrtl.MemBlock.EnabledWrite(DATA_IN, WE & CS)

data_out_wire = pyrtl.wire.WireVector(DATA_WIDTH, "DATA_WIDTH")

with pyrtl.conditional_assignment:
    with OE:
        with WE:
            pass
        with pyrtl.otherwise:
            with CS:
                data_out_wire |= mem[ADDRESS]

DATA_OUT <<= data_out_wire

# Setup the simulation
sim_trace = pyrtl.SimulationTrace()
sim = pyrtl.FastSimulation(tracer=sim_trace, memory_value_map={mem: {0: 0}})
