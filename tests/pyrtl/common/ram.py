#!/usr/bin/env python
# -*- coding: utf-8 -*-

import pyrtl
pyrtl.set_debug_mode(debug=True)

DATA_WIDTH = 8
ADDR_WIDTH = 8
RAM_DEPTH = 1 << 8

address = pyrtl.Input(ADDR_WIDTH, "address")
cs = pyrtl.Input(1, "cs")
we = pyrtl.Input(1, "we")
oe = pyrtl.Input(1, "oe")
data_in = pyrtl.Input(DATA_WIDTH, "data_in")
data_out = pyrtl.Output(DATA_WIDTH, "data_out")

mem = pyrtl.memory.MemBlock(RAM_DEPTH, ADDR_WIDTH,
                            name="mem", asynchronous=True)
mem[address] <<= pyrtl.MemBlock.EnabledWrite(data_in, we & cs)

data_out_wire = pyrtl.wire.WireVector(DATA_WIDTH, "DATA_WIDTH")

with pyrtl.conditional_assignment:
    with oe:
        with we:
            pass
        with pyrtl.otherwise:
            with cs:
                data_out_wire |= mem[address]

data_out <<= data_out_wire

# Setup the simulation
sim_trace = pyrtl.SimulationTrace()
sim = pyrtl.FastSimulation(tracer=sim_trace, memory_value_map={mem: {0: 0}})
