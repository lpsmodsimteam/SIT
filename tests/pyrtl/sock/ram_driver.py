#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import socket
import sys

BASE_DIR = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "common")
sys.path.append(BASE_DIR)
from ram import sim, DATA_OUT

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
    sim.step({
        "ADDRESS": int(signal[0:3]),
        "CS": int(signal[3]),
        "WE": int(signal[4]),
        "OE": int(signal[5]),
        "DATA_IN": int(signal[6:9])
    })
    sock.sendall(str(sim.inspect(DATA_OUT)).encode())
