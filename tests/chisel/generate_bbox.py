#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from boilerplate import Chisel

if __name__ == "__main__":

    boilerplate_obj = Chisel(
        ipc=sys.argv[-1],
        module="ram",
        lib="tests",
        desc="Demonstration of a Chisel hardware simulation in SST",
        lib_dir="../../../../sit/",
        module_dir="../../common/",
    )
    boilerplate_obj.set_ports((
        ("Int", "address", "input"),
        ("Boolean", "cs", "input"),
        ("Boolean", "we", "input"),
        ("Boolean", "oe", "input"),
        ("Int", "data_in", "input"),
        ("Int", "data_out", "output"),
    ))
    boilerplate_obj.generate_bbox()
