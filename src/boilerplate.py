#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import os


class BoilerPlate():

    def __init__(self, module, driver_templ_path,
                 bbox_hpp_templ_path, bbox_cpp_templ_path):

        self.module = module
        self.driver_templ_path = driver_templ_path
        self.bbox_hpp_templ_path = bbox_hpp_templ_path
        self.bbox_cpp_templ_path = bbox_cpp_templ_path

        self.clocks = []
        self.inputs = []
        self.outputs = []
        self.inouts = []
        self.ports = []

        self.lib = ""
        self.comp = ""
        self.desc = ""
        self.link = ""
        self.link_desc = ""

    def set_driver_io(self, ports):

        for port, port_type in ports.items():
            if port_type == "clock":
                self.clocks.append(port)
            elif port_type == "input":
                self.inputs.append(port)
            elif port_type == "output":
                self.outputs.append(port)
            elif port_type == "inout":
                self.inouts.append(port)
            else:
                raise ValueError("Each ports must be designated a type")
        self.ports = ports.keys()

    def get_port_defs(self, formatted=True):

        return ";\n    ".join(self.ports) if formatted else self.ports

    @staticmethod
    def __format(fmt, splitter, array, delim=";\n    ", formatted=True):

        line = (fmt.format(**splitter(i)) for i in array)
        return delim.join(line) if formatted else line

    def get_bindings(self, formatted=True):

        return self.__format(
            "DUT.{p}({p})", lambda x: {"p": x.split(" ")[-1]},
            self.ports, formatted=formatted
        )

    def get_clock(self, formatted=True):

        return self.__format(
            "sh_in.get_clock_pulse(\"{p}\")",
            lambda x: {"p": x.split(" ")[-1]}, self.clocks,
            formatted=formatted
        )

    def get_inputs(self, formatted=True):

        return self.__format(
            "{p} = sh_in.get<{t}>(\"{p}\")",
            lambda x: {
                "p": x.split(" ")[-1],
                "t": x.split(" ")[0]
            }, self.inputs, formatted=formatted
        )

    def get_outputs(self, formatted=True):

        return self.__format(
            "sh_out.set(\"{p}\", {p}, SC_UINT_T)",
            lambda x: {
                "p": x.split(" ")[-1]
            }, self.outputs, formatted=formatted
        )

    def generate_sc_driver(self):

        if os.path.isfile(self.driver_templ_path):
            with open(self.driver_templ_path) as template:
                return template.read().format(
                    module=self.module,
                    port_defs=self.get_port_defs(),
                    bindings=self.get_bindings(),
                    clock=self.get_clock(),
                    inputs=self.get_inputs(),
                    outputs=self.get_outputs()
                )

        raise FileNotFoundError("Driver boilerplate file not found")

    def set_bbox_hpp(self, lib, comp, link, desc="", link_desc=""):

        self.lib = lib
        self.comp = comp
        self.link = link
        self.desc = desc
        self.link_desc = link_desc

    def generate_bbox_hpp(self):

        if os.path.isfile(self.bbox_hpp_templ_path):
            with open(self.bbox_hpp_templ_path) as template:
                return template.read().format(
                    module=self.module,
                    guard=self.module.upper(),
                    lib=self.lib,
                    comp=self.comp,
                    desc=self.desc,
                    link=self.link,
                    link_desc=self.link_desc,
                )

        raise FileNotFoundError("Blackbox HPP boilerplate file not found")


if __name__ == "__main__":

    BASE_DIR = os.getcwd()
    DRIVER_TEMPL_PATH = os.path.join(BASE_DIR, "template", "driver.tmp")
    BBOX_HPP_TEMPL_PATH = os.path.join(BASE_DIR, "template", "blackbox.hpp.tmp")
    BBOX_CPP_TEMPL_PATH = os.path.join(BASE_DIR, "template", "blackbox.cpp.tmp")

    galois = BoilerPlate(
        module="galois_lfsr",
        driver_templ_path=DRIVER_TEMPL_PATH,
        bbox_hpp_templ_path=BBOX_HPP_TEMPL_PATH,
        bbox_cpp_templ_path=BBOX_CPP_TEMPL_PATH,
    )

    galois.set_driver_io({
        "sc_in<bool> clock": "clock",
        "sc_in<bool> reset": "input",
        "sc_out<sc_uint<4> > data_out": "output",
    })
    print(galois.generate_sc_driver())

    galois.set_bbox_hpp("proto", "galois_lfsr", "_link")
    print(galois.generate_bbox_hpp())
