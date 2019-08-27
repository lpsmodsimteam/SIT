#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys

BASE_DIR = os.path.dirname(os.getcwd())
SCRIPT_PATH = os.path.join(BASE_DIR, "ssti", "boilerplate")
DRVR_TEMPL_PATH = os.path.join(SCRIPT_PATH, "template", "systemc", "driver.hpp")
MODEL_TEMPL_PATH = os.path.join(SCRIPT_PATH, "template", "systemc", "model.hpp")
sys.path.append(SCRIPT_PATH)
from gen_systemc import SystemC

if __name__ == "__main__":

    ARGS = dict(
        module_dir="../../common/",
        ports_dir="../../common/blackboxes/",
        lib_dir="../../../../ssti/",
        module="ram",
        lib="systemc",
        drvr_templ_path=DRVR_TEMPL_PATH,
        sst_model_templ_path=MODEL_TEMPL_PATH,
        desc="Demonstration of a SystemC hardware simulation in SST",
        link_desc={
            "link_desc0": "RAM data_in",
            "link_desc1": "RAM data_out",
        }
    )
    boilerplate_obj = SystemC(**ARGS, ipc="sock")
    boilerplate_obj.set_ports((
        ("<sc_bv<ADDR_WIDTH>>//8", "address", "input"),
        ("<bool>", "cs", "input"),
        ("<bool>", "we", "input"),
        ("<bool>", "oe", "input"),
        ("<sc_bv<DATA_WIDTH>>//8", "data_in", "input"),
        ("<sc_bv<DATA_WIDTH>>//8", "data_out", "output"),
    ))
    boilerplate_obj.generate_bbox()
