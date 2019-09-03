#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys

import zmq

sys.path.append("../../common/")
import ram

# Connect the PyRTL simulation to SST through Unix sockets
context = zmq.Context()
_sock = context.socket(zmq.REQ)
_sock.connect(sys.argv[1])

_sock.send(str(os.getpid()).encode())

while True:
    signal = str(_sock.recv().decode("utf-8"))
    alive = int(signal[0])
    signal = signal[1:]
    if not alive:
        break
    ram.sim.step({
        "address": int(signal[0:3]),
        "cs": int(signal[3:4]),
        "we": int(signal[4:5]),
        "oe": int(signal[5:6]),
        "data_in": int(signal[6:9]),
    })
    _outputs = str(ram.sim.inspect(ram.data_out)).encode()
    _sock.send(_outputs)
