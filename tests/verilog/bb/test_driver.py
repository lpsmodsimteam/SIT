#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import socket

import cocotb
from cocotb.triggers import Timer


@cocotb.test()
def ram_test(dut):

    # Connect the PyRTL simulation to SST through socket
    _sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    _sock.connect(cocotb.plusargs["ipc_proc"])

    _sock.sendall(str(os.getpid()).encode())

    while True:
        signal = str(_sock.recv(10).decode("utf-8"))
        alive = int(signal[0])
        signal = signal[1:]
        if not alive:
            break
        dut.address = int(signal[0:3])
        dut.cs = int(signal[3:4])
        dut.we = int(signal[4:5])
        dut.oe = int(signal[5:6])
        dut.data_in = int(signal[6:9])

        yield Timer(1, units='ns')
        _sock.sendall(dut.data_out.encode())
