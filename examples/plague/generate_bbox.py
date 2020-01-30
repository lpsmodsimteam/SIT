#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from boilerplate import PyRTL, SystemC

if __name__ == "__main__":

    if sys.argv[-1] == "systemc":
        systemc_obj = SystemC(
            lib="plague",
            module_dir="../",
            ipc="sock",
            lib_dir="../../../../sit/",
            module="randf",
        )
        systemc_obj.set_ports((
            ("<bool>", "reseed", "input"),
            ("<sc_uint<16>>", "seed", "input"),
            ("<sc_uint<8>>", "lower_limit", "input"),
            ("<sc_uint<16>>", "upper_limit", "input"),
            ("<float>", "data_out", "output"),
        ))
        systemc_obj.generate_bbox()

        systemc_obj = SystemC(
            lib="plague",
            module_dir="../",
            ipc="sock",
            lib_dir="../../../../sit/",
            module="rng",
        )
        systemc_obj.set_ports((
            ("<bool>", "reseed", "input"),
            ("<sc_uint<16>>", "seed", "input"),
            ("<sc_uint<8>>", "lower_limit", "input"),
            ("<sc_uint<16>>", "upper_limit", "input"),
            ("<sc_uint<16>>", "data_out", "output"),
        ))
        systemc_obj.generate_bbox()

    # elif sys.argv[-1] == "pyrtl":
    #     pyrtl_obj = PyRTL(
    #         lib="intersection",
    #         module_dir="../",
    #         ipc="sock",
    #         lib_dir="../../../../sit/",
    #         module="pyrtl_fsm",
    #     )
    #     pyrtl_obj.set_ports((
    #         ("1", "load", "input"),
    #         ("1", "start_green", "input"),
    #         ("6", "green_time", "input"),
    #         ("2", "yellow_time", "input"),
    #         ("6", "red_time", "input"),
    #         ("2", "state", "output"),
    #     ))
    #     pyrtl_obj.generate_bbox()
