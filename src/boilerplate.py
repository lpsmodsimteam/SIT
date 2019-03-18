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

        self.clocks = []
        self.inputs = []
        self.outputs = []
        self.inouts = []
        self.ports = []
        self.driver_var_decl = ""

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

        if self.ipc == "zmq":
            self.driver_var_decl = """// Socket to talk to server
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect(argv[1]);

    // Initialize signal handlers
    ZMQReceiver sh_in(socket);
    ZMQTransmitter sh_out(socket);"""
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

    def set_ports(self, ports):

        for port, port_type in ports.items():
            if port_type == "clock":
                self.clocks.append(port)
            elif port_type == "input":
                self.inputs.append(port.split(" "))
            elif port_type == "output":
                self.outputs.append(port)
            elif port_type == "inout":
                self.inouts.append(port)
            else:
                raise ValueError("Each ports must be designated a type")
            self.ports.append("sc_signal" + port)

    @staticmethod
    def __parse_signal_type(signal):

        if "sc" in signal:

            if "int" in signal or "uint" in signal or "bv" in signal:
                return "<int>", int("".join(s for s in signal if s.isdigit()))

            if "bit" in signal or "logic" in signal:
                return "<bool>", int("".join(s for s in signal if s.isdigit()))

        else:

            return signal, 1

    @staticmethod
    def __format(fmt, split_func, array, delim=";\n    "):

        return delim.join(fmt.format(**split_func(i)) for i in array)

    def get_port_defs(self):

        return ";\n    ".join(self.ports)

    def get_bindings(self):

        return self.__format(
            "DUT.{sig}({sig})",
            lambda x: {"sig": x.split(" ")[-1]}, self.ports
        )

    def get_clock(self):

        return self.__format(
            "sh_in.get_clock_pulse(\"{sig}\")",
            lambda x: {"sig": x.split(" ")[-1]}, self.clocks
        )

    def get_inputs(self, driver=True):

        return self.__format(
            "{sig} = sh_in.get{type}(\"{sig}\")",
            lambda x: {
                "sig": x[-1], "type": self.__parse_signal_type(x[0])[0]},
            self.inputs, ";\n" + " " * 8
        ) if driver else self.__format(
            "std::to_string(m_sh_in.get{type}(\"{sig}\"))",
            lambda x: {
                "sig": x.split(" ")[-1],
                "type": self.__parse_signal_type(x.split(" ")[0])[0],
            }, self.outputs, " +\n" + " " * 16
        )

    def get_outputs(self, driver=True):

        if driver:

            return self.__format(
                "sh_out.set(\"{sig}\", {sig})",
                lambda x: {"sig": x.split(" ")[-1]},
                self.outputs, ";\n" + " " * 8
            )

        else:

            ix = 2
            sig_lens = []
            for i in self.inputs:
                sig_len = self.__parse_signal_type(i[0])[-1]
                sig_lens.append((ix, sig_len))
                ix += sig_len

            return self.__format(
                "m_sh_out.set(\"{sig}\", std::stoi(_data_in.substr({p}, {l})))",
                lambda x: {
                    "sig": x[-1],
                    "p": sig_lens[self.inputs.index(x)][0],
                    "l": sig_lens[self.inputs.index(x)][-1]
                }, self.inputs, ";\n" + " " * 8
            )

    def generate_sc_driver(self):

        if os.path.isfile(self.driver_templ_path):
            with open(self.driver_templ_path) as template:
                return template.read().format(
                    module=self.module,
                    port_defs=self.get_port_defs(),
                    bindings=self.get_bindings(),
                    var_decl=self.driver_var_decl,
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
                    inputs=self.get_inputs(False),
                    outputs=self.get_outputs(False)
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

    galois.set_ports({
        "<bool> clock": "clock",
        "<bool> reset": "input",
        "<bool> reset1": "input",
        "<bool> reset2": "input",
        "<sc_uint<32> > din": "input",
        "<sc_uint<4> > data_out": "output",
        "<sc_uint<4> > data_out1": "output",
    })
    print(galois.generate_sc_driver())

    galois.set_bbox("proto", "galois_lfsr", "_link")
    print(galois.generate_bbox())
