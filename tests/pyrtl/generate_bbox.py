#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys

BASE_DIR = os.path.dirname(os.path.dirname(
    os.path.dirname(os.path.abspath(__file__))))
SCRIPT_PATH = os.path.join(BASE_DIR, "ssti")
sys.path.append(SCRIPT_PATH)
from boilerplate import PyRTL

if __name__ == "__main__":

    ARGS = dict(
        module_dir="../../common/",
        lib_dir="../../../../ssti/",
        module="ram",
        lib="tests",
        desc="Demonstration of a PyRTL hardware simulation in SST",
        link_desc={
            "link_desc0": "RAM data_in",
            "link_desc1": "RAM data_out",
        }
    )

    boilerplate_obj = PyRTL(**ARGS, ipc=sys.argv[-1])
    boilerplate_obj.set_ports((
        ("8", "address", "input"),
        ("1", "cs", "input"),
        ("1", "we", "input"),
        ("1", "oe", "input"),
        ("8", "data_in", "input"),
        ("8", "data_out", "output"),
    ))
    boilerplate_obj.generate_bbox()
