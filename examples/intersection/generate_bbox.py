#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from boilerplate import PyRTL, SystemC

if __name__ == "__main__":

    args = {
        "lib": "intersection",
        "module_dir": "../",
        "ipc": "sock",
        "lib_dir": "../../../../sit/",
    }

    if sys.argv[-1] == "systemc":
        systemc_obj = SystemC(
            **args,
            module="systemc_fsm",
        )
        systemc_obj.set_ports((
            ("<bool>//5", "clock", "clock"),
            ("<bool>", "load", "input"),
            ("<bool>", "start_green", "input"),
            ("<sc_uint<6>>", "green_time", "input"),
            ("<sc_uint<2>>", "yellow_time", "input"),
            ("<sc_uint<6>>", "red_time", "input"),
            ("<sc_uint<2>>", "state", "output"),
        ))
        systemc_obj.generate_bbox()

    elif sys.argv[-1] == "pyrtl":
        pyrtl_obj = PyRTL(
            **args,
            module="pyrtl_fsm",
        )
        pyrtl_obj.set_ports((
            ("1", "load", "input"),
            ("1", "start_green", "input"),
            ("6", "green_time", "input"),
            ("2", "yellow_time", "input"),
            ("6", "red_time", "input"),
            ("2", "state", "output"),
        ))
        pyrtl_obj.generate_bbox()
