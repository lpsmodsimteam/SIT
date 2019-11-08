#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from boilerplate import PyRTL

if __name__ == "__main__":
    boilerplate_obj = PyRTL(
        module="ram",
        lib="tests",
        ipc=sys.argv[-1],
        module_dir="../../common/",
        lib_dir="../../../../ssti/",
        desc="Demonstration of a PyRTL hardware simulation in SST",
    )
    boilerplate_obj.set_ports((
        ("8", "address", "input"),
        ("1", "cs", "input"),
        ("1", "we", "input"),
        ("1", "oe", "input"),
        ("8", "data_in", "input"),
        ("8", "data_out", "output"),
    ))
    boilerplate_obj.generate_bbox()
