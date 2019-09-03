#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys

BASE_DIR = os.path.dirname(os.path.dirname(
    os.path.dirname(os.path.abspath(__file__))))
SCRIPT_PATH = os.path.join(BASE_DIR, "ssti")
BPLATE_DIR = os.path.join(SCRIPT_PATH, "boilerplate", "template", "pyrtl")
DRVR_TEMPL_PATH = os.path.join(BPLATE_DIR, "driver.py")
MODEL_TEMPL_PATH = os.path.join(BPLATE_DIR, "model.cpp")
sys.path.append(SCRIPT_PATH)
from boilerplate import PyRTL

if __name__ == "__main__":

    ARGS = dict(
        module_dir="../../common/",
        lib_dir="../../../../ssti/",
        module="ram",
        lib="tests",
        drvr_templ_path=DRVR_TEMPL_PATH,
        sst_model_templ_path=MODEL_TEMPL_PATH,
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
