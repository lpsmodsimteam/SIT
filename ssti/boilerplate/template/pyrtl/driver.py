#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import socket
import sys

sys.path.append({base_dir})
from {module} import sim, {outputs}

# Connect the PyRTL simulation to SST through Unix sockets
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
sock.connect(sys.argv[1])

sock.sendall(str(os.getpid()).encode())

while True:
    signal = str(sock.recv({sig_len}).decode("utf-8"))
    alive = int(signal[0])
    signal = signal[1:]
    if not alive:
        break
    sim.step({{
        {inputs}
    }})
    sock.sendall(str(sim.inspect({outputs})).encode())
