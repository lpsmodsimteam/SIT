#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os


class BoilerPlate():

    def __init__(self, module, ipc, driver_templ_path, bbox_templ_path):

        self.module = module
        if ipc in ("socks", "zmq"):
            self.ipc = ipc
        else:
            raise ValueError("Incorrect IPC protocol selected")
        self.driver_templ_path = driver_templ_path
        self.bbox_templ_path = bbox_templ_path

        self._delim = ";\n    "

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

        self.bbox_var_decl = ""
        self.bbox_var_init = ""
        self.bbox_var_bind = ""
        self.bbox_var_dest = ""
        self.bbox_sender = ""
        self.bbox_receiver = ""
        self.bbox_inputs = ""
        self.bbox_outputs = ""

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

    def get_port_defs(self):

        return self._delim.join(self.ports)

    def __format(self, fmt, splitter, array):

        return self._delim.join(fmt.format(**splitter(i)) for i in array)

    def get_bindings(self):

        return self.__format(
            "DUT.{p}({p})", lambda x: {"p": x.split(" ")[-1]},
            self.ports
        )

    def get_clock(self):

        return self.__format(
            "sh_in.get_clock_pulse(\"{p}\")",
            lambda x: {"p": x.split(" ")[-1]}, self.clocks
        )

    def get_inputs(self, driver=True):

        return self.__format(
            "{p} = sh_in.get<{t}>(\"{p}\")",
            lambda x: {
                "p": x.split(" ")[-1],
                "t": x.split(" ")[0]
            }, self.inputs
        ) if driver else None

    def get_outputs(self, driver=True):

        return self.__format(
            "sh_out.set(\"{p}\", {p}, SC_UINT_T)",
            lambda x: {
                "p": x.split(" ")[-1]
            }, self.outputs
        ) if driver else None

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

    def set_bbox(self, lib, comp, link, desc="", link_desc=""):

        self.lib = lib
        self.comp = comp
        self.link = link
        self.desc = desc
        self.link_desc = link_desc

        if self.ipc == "zmq":
            self.bbox_var_decl = """zmq::context_t m_context;
    zmq::socket_t m_socket;
    ZMQReceiver m_sh_in;
    ZMQTransmitter m_sh_out;"""
            self.bbox_var_init = """m_context(1), m_socket(m_context, ZMQ_REP),
      m_sh_in(m_socket), m_sh_out(m_socket),"""
            self.bbox_var_bind = "m_socket.bind(m_ipc_port.c_str())"
            self.bbox_var_dest = ""
            self.bbox_sender = "m_sh_in"
            self.bbox_receiver = "m_sh_out"
            self.bbox_inputs = []
            self.bbox_outputs = []

    def generate_bbox(self):

        if os.path.isfile(self.bbox_templ_path):
            with open(self.bbox_templ_path) as template:
                return template.read().format(
                    module=self.module,
                    lib=self.lib,
                    comp=self.comp,
                    desc=self.desc,
                    var_decl=self.bbox_var_decl,
                    var_init=self.bbox_var_init,
                    var_bind=self.bbox_var_bind,
                    var_dest=self.bbox_var_dest,
                    sender=self.bbox_sender,
                    receiver=self.bbox_receiver,
                    inputs=self.bbox_inputs,
                    outputs=self.bbox_outputs
                )

        raise FileNotFoundError("Blackbox boilerplate file not found")


if __name__ == "__main__":

    BASE_DIR = os.getcwd()
    DRIVER_TEMPL_PATH = os.path.join(BASE_DIR, "template", "driver.tmp")
    BBOX_TEMPL_PATH = os.path.join(BASE_DIR, "template", "blackbox.tmp")

    galois = BoilerPlate(
        module="galois_lfsr",
        ipc="zmq",
        driver_templ_path=DRIVER_TEMPL_PATH,
        bbox_templ_path=BBOX_TEMPL_PATH
    )

    galois.set_driver_io({
        "sc_in<bool> clock": "clock",
        "sc_in<bool> reset": "input",
        "sc_out<sc_uint<4> > data_out": "output",
    })
    print(galois.generate_sc_driver())

    galois.set_bbox("proto", "galois_lfsr", "_link")
    print(galois.generate_bbox())
