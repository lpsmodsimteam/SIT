#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from boilerplate import SystemC

if __name__ == "__main__":

    ipc = sys.argv[-1]

    boilerplate_obj = SystemC(
        ipc=ipc,
        module="ram",
        lib="systemc" + ipc,
        width_macros={
            "ADDR_WIDTH": 8,
            "DATA_WIDTH": 8,
        },
        desc="Demonstration of a SystemC hardware simulation in SST",
        module_dir="../../tests/systemc/common/",
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
