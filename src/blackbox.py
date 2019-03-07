#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import os

BASE_DIR = os.getcwd()
TEMPL_PATH = os.path.join(BASE_DIR, "template", "driver.tmp")


class SystemC():

    def __init__(self, module, templ_path):

        self.module = module
        self.templ_path = templ_path
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

    def generate_sc_driver(self):

        with open(self.templ_path) as template:
            print(
                template.read().format(
                    module=self.module,
                    port_defs=self.get_port_defs(),
                    bindings=self.get_bindings(),
                    clock=self.get_clock(),
                    inputs=self.get_inputs(),
                    outputs=self.get_outputs()
                )
            )


if __name__ == '__main__':

    galois = SystemC(module="galois_lfsr", templ_path=TEMPL_PATH)

    galois.set_io({
        "sc_in<bool> clock": "clock",
        "sc_in<bool> reset": "input",
        "sc_out<sc_uint<4> > data_out": "output",
    })
    galois.generate_sc_driver()
