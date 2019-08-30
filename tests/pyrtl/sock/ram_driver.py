#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import socket
import sys

BASE_DIR = os.path.join(os.path.dirname(
    os.path.dirname(os.path.abspath(__file__))), "common")
sys.path.append(BASE_DIR)
import ram

# Connect the PyRTL simulation to SST through Unix sockets
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
sock.connect(sys.argv[1])

sock.sendall(str(os.getpid()).encode())

while True:
    signal = str(sock.recv(10).decode("utf-8"))
    alive = int(signal[0])
    signal = signal[1:]
    if not alive:
        break
    ram.sim.step({
        "address": int(signal[0:3]),
        "cs": int(signal[3]),
        "we": int(signal[4]),
        "oe": int(signal[5]),
        "data_in": int(signal[6:9])
    })
    _data_out = str(ram.sim.inspect(ram.data_out)).encode()
    sock.sendall(_data_out)
