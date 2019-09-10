#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import socket
import sys

from traffic_light_fsm import sim, state

# Connect the PyRTL simulation to SST through Unix sockets
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
sock.connect(sys.argv[1])

sock.sendall(str(os.getpid()).encode())

while True:
    signal = str(sock.recv(9).decode("utf-8"))
    alive = int(signal[0])
    signal = signal[1:]

    if not alive:
        break
    sim.step({
        "load": int(signal[0]),
        "start_green": int(signal[1]),
        "green_time": int(signal[2:4]),
        "yellow_time": int(signal[4:6]),
        "red_time": int(signal[6:8])
    })
    sock.sendall(str(sim.inspect(state)).encode())
