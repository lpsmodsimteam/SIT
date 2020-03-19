#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import socket

import cocotb
from cocotb.triggers import Timer


@cocotb.test()
def ram_test(dut):

    print(cocotb.plusargs["ipc_proc"])
    # Connect the PyRTL simulation to SST through socket
    _sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    _sock.connect(cocotb.plusargs["ipc_proc"])

    print("sending", os.getpid())
    _sock.sendall(str(os.getpid()).encode())

    while True:
        signal = str(_sock.recv(10).decode("utf-8"))
        alive = int(signal[0])
        signal = signal[1:]
        if not alive:
            break
        dut.addr = int(signal[0:3])
        dut.cs = int(signal[3:4])
        dut.we = int(signal[4:5])
        dut.oe = int(signal[5:6])
        dut.data_in = int(signal[6:9])

        yield Timer(1, units='ns')
        print("data_out", str(dut.data_out))
        _sock.sendall(str(dut.data_out).encode())

    _sock.close()
