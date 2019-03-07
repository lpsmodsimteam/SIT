#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import os

BASE_DIR = os.getcwd()
TEMPL_PATH = os.path.join(BASE_DIR, "template", "driver.tmp")
MOD_PATH = os.path.join(BASE_DIR, "galois_lfsr.hpp")


class SystemC():

    def __init__(self, path):

        self.path = path
        with open(self.path) as module_file:
            self.file = module_file.read()

        self.clocks = []
        self.inputs = []
        self.outputs = []
        self.inouts = []

    def set_io(self, ports):

        for port, port_type in ports.items():
            if port_type == "clock":
                self.clocks.append(port)
            if port_type == "input":
                self.inputs.append(port)
            elif port_type == "output":
                self.outputs.append(port)
            elif port_type == "inout":
                self.inouts.append(port)
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


if __name__ == '__main__':

    galois = SystemC(MOD_PATH)

    galois.set_io({
        "sc_in<bool> clock": "clock",
        "sc_in<bool> reset": "input",
        "sc_out<sc_uint<4> > data_out": "output",
    })

    with open(TEMPL_PATH) as template:
        print(
            template.read().format(
                module="galois_lfsr",
                port_defs=galois.get_port_defs(),
                bindings=galois.get_bindings(),
                clock=galois.get_clock(),
                inputs=galois.get_inputs(),
                outputs=galois.get_outputs()
            )
        )
