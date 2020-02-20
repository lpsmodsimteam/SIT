#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from boilerplate import SystemC

if __name__ == "__main__":

    boilerplate_obj = SystemC(
        ipc=sys.argv[-1],
        module="ram",
        lib="tests",
        width_macros={
            "ADDR_WIDTH": 8,
            "DATA_WIDTH": 8,
        },
        desc="Demonstration of a SystemC hardware simulation in SST",
        lib_dir="../../../../sit/",
        module_dir="../../common/",
    )
    boilerplate_obj.set_ports((
        ("input", "address", "<sc_bv<ADDR_WIDTH>>"),
        ("input", "cs", "<bool>"),
        ("input", "we", "<bool>"),
        ("input", "oe", "<bool>"),
        ("input", "data_in", "<sc_bv<DATA_WIDTH>>"),
        ("output", "data_out", "<sc_bv<DATA_WIDTH>>", 8),
    ))
    boilerplate_obj.generate_bbox()
