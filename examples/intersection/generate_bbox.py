#!/usr/bin/env python
# -*- coding: utf-8 -*-

from boilerplate import SystemC

if __name__ == "__main__":

    boilerplate_obj = SystemC(
        module="traffic_light_fsm",
        lib="intersection",
        ipc="sock",
        lib_dir="../../ssti/",
        module_dir="../systemc/",
        desc="Demonstration of a SystemC hardware simulation in SST",
        link_desc={
            "link_desc0": "Traffic Light FSM data_in",
            "link_desc1": "Traffic Light FSM data_out",
        }
    )
    boilerplate_obj.set_ports((
        ("<bool>", "clock", "clock"),
        ("<bool>", "load", "input"),
        ("<bool>", "start_green", "input"),
        ("<sc_uint<6>>", "green_time", "input"),
        ("<sc_uint<2>>", "yellow_time", "input"),
        ("<sc_uint<6>>", "red_time", "input"),
        ("<sc_uint<2>>", "state", "output"),
    ))
    boilerplate_obj.generate_bbox()
