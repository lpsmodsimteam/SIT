#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from boilerplate import PyRTL

if __name__ == "__main__":

    boilerplate_obj = PyRTL(
        ipc=sys.argv[-1],
        module="ram",
        lib="tests",
        desc="Demonstration of a PyRTL hardware simulation in SST",
        lib_dir="../../../../sit/",
        module_dir="../../common/",
    )
    boilerplate_obj.set_ports((
        ("input", "address", "8"),
        ("input", "cs", "1"),
        ("input", "we", "1"),
        ("input", "oe", "1"),
        ("input", "data_in", "8"),
        ("output", "data_out", "8"),
    ))
    boilerplate_obj.generate_bbox()
