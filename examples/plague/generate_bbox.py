#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from boilerplate import PyRTL, SystemC

if __name__ == "__main__":

    ARGS = {
        "lib": "plague",
        "module_dir": "../",
        "ipc": "sock",
        "lib_dir": "../../../../sit/",
    }

    if sys.argv[-1] == "systemc":
        randf = SystemC(
            **ARGS,
            width_macros={
                "clock": 2,
            },
            module="randf",
        )
        randf.set_ports((
            ("<bool>", "clock", "clock"),
            ("<sc_uint<16>>", "seed", "input"),
            ("<sc_uint<8>>", "lower_limit", "input"),
            ("<sc_uint<16>>", "upper_limit", "input"),
            ("<float>", "data_out", "output"),
        ))
        randf.generate_bbox()

        minf = SystemC(
            **ARGS,
            module="minf",
        )
        minf.set_ports((
            ("<float>", "operand1", "input"),
            ("<float>", "operand2", "input"),
            ("<float>", "data_out", "output"),
        ))
        minf.generate_bbox()

    elif sys.argv[-1] == "pyrtl":
        mutation = PyRTL(
            **ARGS,
            module="mutation",
        )
        mutation.set_ports((
            ("4", "chance", "input"),
            ("4", "gene", "input"),
            ("2", "data_out", "output"),
        ))
        mutation.generate_bbox()
