#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from boilerplate import Chisel, PyRTL, SystemC

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
            module="randf",
        )
        randf.set_ports((
            ("clock", "clock", "<bool>", 5),
            ("input", "en", "<bool>"),
            ("input", "seed", "<sc_uint<16>>"),
            ("input", "lower_limit", "<sc_uint<8>>"),
            ("input", "upper_limit", "<sc_uint<10>>"),
            ("output", "data_out", "<float>", 12),
        ))
        randf.fixed_width_float_output(9)
        randf.disable_runtime_warnings(["SC_ID_NO_SC_START_ACTIVITY_"])
        randf.generate_bbox()

        minf = SystemC(
            **ARGS,
            module="minf",
        )
        minf.set_ports((
            ("input", "operand1", "<float>", 12),
            ("input", "operand2", "<float>", 5),
            ("output", "data_out", "<float>", 12),
        ))
        minf.fixed_width_float_output(9)
        minf.disable_runtime_warnings(["SC_ID_NO_SC_START_ACTIVITY_"])
        minf.generate_bbox()

        sc_exp = SystemC(
            **ARGS,
            module="sc_exp",
        )
        sc_exp.set_ports((
            ("input", "operand", "<float>", 12),
            ("output", "data_out", "<float>", 12),
        ))
        sc_exp.fixed_width_float_output(9)
        sc_exp.disable_runtime_warnings(["SC_ID_NO_SC_START_ACTIVITY_"])
        sc_exp.generate_bbox()

        rng = SystemC(
            **ARGS,
            module="rng",
        )
        rng.set_ports((
            ("clock", "clock", "<bool>", 5),
            ("input", "en", "<bool>"),
            ("input", "seed", "<sc_uint<16>>"),
            ("input", "lower_limit", "<sc_uint<8>>"),
            ("input", "upper_limit", "<sc_uint<10>>"),
            ("output", "data_out", "<sc_uint<10>>"),
        ))
        rng.disable_runtime_warnings(["SC_ID_NO_SC_START_ACTIVITY_"])
        rng.generate_bbox()

        sc_ceil = SystemC(
            **ARGS,
            module="sc_ceil",
        )
        sc_ceil.set_ports((
            ("input", "operand", "<float>", 12),
            ("output", "data_out", "<sc_uint<25>>"),
        ))
        sc_ceil.disable_runtime_warnings(["SC_ID_NO_SC_START_ACTIVITY_"])
        sc_ceil.generate_bbox()

    elif sys.argv[-1] == "pyrtl":
        mutation = PyRTL(
            **ARGS,
            module="mutation",
        )
        mutation.set_ports((
            ("input", "chance", "3"),
            ("input", "gene", "3"),
            ("output", "out", "2"),
        ))
        mutation.generate_bbox()

    elif sys.argv[-1] == "chisel":

        flash_mem = Chisel(
            **ARGS,
            module="flash_mem",
        )
        flash_mem.set_ports((
            ("input", "address", "25"),
            ("input", "cs", "1"),
            ("input", "we", "1"),
            ("input", "oe", "1"),
            ("input", "data_in", "25"),
            ("output", "data_out", "25"),
        ))
        flash_mem.generate_bbox()
