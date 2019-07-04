#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import socket
import sys

from traffic_light_fsm import sim, out

# Connect the PyRTL simulation to SST through Unix sockets
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
sock.connect(sys.argv[1])

sock.sendall(str(os.getpid()).encode())

while True:
    cmd = str(sock.recv(9).decode("utf-8"))
    alive = int(cmd[0])
    cmd = cmd[1:]

    if not alive:
        break
    sim.step({
        "LOAD": int(cmd[0]),
        "STARTGREEN": int(cmd[1]),
        "GREENTIME": int(cmd[2:4]),
        "YELLOWTIME": int(cmd[4:6]),
        "REDTIME": int(cmd[6:8])
    })
    sock.sendall(str(sim.inspect(out)).encode())
