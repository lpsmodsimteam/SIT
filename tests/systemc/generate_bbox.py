#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from boilerplate import SystemC

if __name__ == "__main__":

    boilerplate_obj = SystemC(
        ipc=sys.argv[-1],
        module="ram",
        lib="tests",
        macros={
            "ADDR_WIDTH": "8",
            "DATA_WIDTH": "8"
        },
        desc="Demonstration of a SystemC hardware simulation in SST",
        lib_dir="../../../../sit/",
        module_dir="../../common/",
    )
    boilerplate_obj.set_ports((
        ("<sc_bv<ADDR_WIDTH>>", "address", "input"),
        ("<bool>", "cs", "input"),
        ("<bool>", "we", "input"),
        ("<bool>", "oe", "input"),
        ("<sc_bv<DATA_WIDTH>>", "data_in", "input"),
        ("<sc_bv<DATA_WIDTH>>", "data_out", "output"),
    ))
    boilerplate_obj.generate_bbox()
