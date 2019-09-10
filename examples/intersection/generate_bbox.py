#!/usr/bin/env python
# -*- coding: utf-8 -*-

from boilerplate import PyRTL, SystemC

if __name__ == "__main__":

    args = {
        "module": "traffic_light_fsm",
        "lib": "intersection",
        "ipc": "sock",
        "lib_dir": "../../ssti/",
        "link_desc": {
            "link_desc0": "Traffic Light FSM data_in",
            "link_desc1": "Traffic Light FSM data_out",
        }
    }

    systemc_obj = SystemC(
        **args,
        module_dir="../systemc/",
    )
    systemc_obj.set_ports((
        ("<bool>", "clock", "clock"),
        ("<bool>", "load", "input"),
        ("<bool>", "start_green", "input"),
        ("<sc_uint<6>>", "green_time", "input"),
        ("<sc_uint<2>>", "yellow_time", "input"),
        ("<sc_uint<6>>", "red_time", "input"),
        ("<sc_uint<2>>", "state", "output"),
    ))
    systemc_obj.generate_bbox()

    pyrtl_obj = PyRTL(
        **args
    )
    pyrtl_obj.set_ports((
        ("1", "load", "input"),
        ("1", "start_green", "input"),
        ("6", "green_time", "input"),
        ("2", "yellow_time", "input"),
        ("6", "red_time", "input"),
        ("2", "state", "output"),
    ))
    pyrtl_obj.generate_bbox()
