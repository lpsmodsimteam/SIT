#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from boilerplate import SystemC

if __name__ == "__main__":

    boilerplate_obj = SystemC(
        module="ram",
        lib="tests",
        ipc=sys.argv[-1],
        lib_dir="../../../../ssti/",
        module_dir="../../common/",
        desc="Demonstration of a SystemC hardware simulation in SST",
    )
    boilerplate_obj.set_ports((
        ("<sc_bv<ADDR_WIDTH>>//8", "address", "input"),
        ("<bool>", "cs", "input"),
        ("<bool>", "we", "input"),
        ("<bool>", "oe", "input"),
        ("<sc_bv<DATA_WIDTH>>//8", "data_in", "input"),
        ("<sc_bv<DATA_WIDTH>>//8", "data_out", "output"),
    ))
    boilerplate_obj.generate_bbox()
