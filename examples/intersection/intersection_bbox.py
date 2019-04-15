#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys

BASE_DIR = os.path.dirname(os.path.dirname(os.getcwd()))
SCRIPT_PATH = os.path.join(BASE_DIR, "src", "boilerplate")
BBOX_DIR_PATH = os.path.join(BASE_DIR, "examples", "simple", "blackboxes")
DRVR_TEMPL_PATH = os.path.join(SCRIPT_PATH, "template", "driver.tmp")
MODEL_TEMPL_PATH = os.path.join(SCRIPT_PATH, "template", "model.tmp")
sys.path.append(SCRIPT_PATH)
from generate import BoilerPlate

if __name__ == "__main__":

    boilerplate_obj = BoilerPlate(
        module="stoplight",
        lib="intersection",
        ipc="sock",
        drvr_templ_path=DRVR_TEMPL_PATH,
        sst_model_templ_path=MODEL_TEMPL_PATH,
        desc="Demonstration of a SystemC hardware simulation in SST",
        link_desc={
            "link_desc0": "Stoplight data_in",
            "link_desc1": "Stoplight data_out",
        }
    )
    boilerplate_obj.set_ports({
        "<bool> clock": "clock",
        "<bool> load": "input",
        "<bool> start_green": "input",
        "<sc_uint<6> > green_time": "input",
        "<sc_uint<6> > yellow_time": "input",
        "<sc_uint<6> > red_time": "input",
        "<light_state> state": "output",
    })
    boilerplate_obj.generate_bbox()
