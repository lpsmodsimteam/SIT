#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os


class BoilerPlate():

    def __init__(self, module, lib, ipc, drvr_templ_path, bbox_templ_path,
                 desc=""):

        if ipc in ("socks", "zmq"):
            self.ipc = ipc
        else:
            raise ValueError("Incorrect IPC protocol selected")

        self.module = module
        self.lib = lib
        self.drvr_templ_path = drvr_templ_path
        self.bbox_templ_path = bbox_templ_path
        self.desc = desc

        self.clocks = []
        self.inputs = []
        self.outputs = []
        self.inouts = []
        self.ports = []

        self.drvr_decl = ""

        self.bbox_bind = ""
        self.bbox_decl = ""
        self.bbox_dest = ""
        self.bbox_init = ""
        self.bbox_links = []
        self.bbox_receiver = ""
        self.bbox_sender = ""

        if self.ipc == "zmq":
            self.drvr_decl = """// Socket to talk to server
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect(argv[1]);

    // Initialize signal handlers
    ZMQReceiver sh_in(socket);
    ZMQTransmitter sh_out(socket);"""
            self.bbox_decl = """zmq::context_t m_context;
    zmq::socket_t m_socket;
    ZMQReceiver m_sh_in;
    ZMQTransmitter m_sh_out;"""
            self.bbox_init = """m_context(1), m_socket(m_context, ZMQ_REP),
      m_sh_in(m_socket), m_sh_out(m_socket),"""
            self.bbox_bind = "m_socket.bind(m_ipc_port.c_str())"
            self.bbox_dest = "m_socket.close();"
            self.bbox_sender = "m_sh_in"
            self.bbox_receiver = "m_sh_out"

    def set_ports(self, ports):

        for port, port_type in ports.items():
            if port_type == "clock":
                self.clocks.append(port.split(" "))
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

            def __get_ints(sig):

                return int("".join(s for s in sig if s.isdigit()))

            if "int" in signal or "bv" in signal or "lv" in signal:
                return "<int>", __get_ints(signal)

            if "bit" in signal or "logic" in signal:
                return "<bool>", __get_ints(signal)

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

    def get_clock(self, drvr=True):

        return self.__format(
            "{sig} = sh_in.get_clock_pulse(\"{sig}\")",
            lambda x: {"sig": x[-1]}, self.clocks
        ) if drvr else [["<sc_int<2>>", i[-1]] for i in self.clocks]

    def get_inputs(self, drvr=True):

        return self.__format(
            "{sig} = sh_in.get{type}(\"{sig}\")",
            lambda x: {
                "sig": x[-1], "type": self.__parse_signal_type(x[0])[0]
            }, self.inputs, ";\n" + " " * 8
        ) if drvr else self.__format(
            "std::to_string(m_sh_in.get{type}(\"{sig}\"))",
            lambda x: {
                "sig": x.split(" ")[-1],
                "type": self.__parse_signal_type(x.split(" ")[0])[0],
            }, self.outputs, " +\n" + " " * 16
        )

    def get_outputs(self, drvr=True):

        if drvr:

            return self.__format(
                "sh_out.set(\"{sig}\", {sig})",
                lambda x: {"sig": x.split(" ")[-1]},
                self.outputs, ";\n" + " " * 8
            )

        ix = 2
        sig_lens = []
        bbox_inputs = self.inputs + self.get_clock(False)
        for i in bbox_inputs:
            sig_len = self.__parse_signal_type(i[0])[-1]
            sig_lens.append((ix, sig_len))
            ix += sig_len

        return self.__format(
            "m_sh_out.set(\"{sig}\", std::stoi(_data_in.substr({p}, {l})))",
            lambda x: {
                "sig": x[-1],
                "p": sig_lens[bbox_inputs.index(x)][0],
                "l": sig_lens[bbox_inputs.index(x)][-1]
            }, bbox_inputs, ";\n" + " " * 8
        )

    def generate_sc_drvr(self):

        if os.path.isfile(self.drvr_templ_path):
            with open(self.drvr_templ_path) as template:
                return template.read().format(
                    module=self.module,
                    port_defs=self.get_port_defs(),
                    bindings=self.get_bindings(),
                    var_decl=self.drvr_decl,
                    clock=self.get_clock(),
                    inputs=self.get_inputs(),
                    outputs=self.get_outputs()
                )

        raise FileNotFoundError("Driver boilerplate file not found")

    def generate_bbox(self):

        if os.path.isfile(self.bbox_templ_path):
            with open(self.bbox_templ_path) as template:
                return template.read().format(
                    module=self.module,
                    lib=self.lib,
                    comp=self.module,
                    desc=self.desc,
                    var_decl=self.bbox_decl,
                    var_init=self.bbox_init,
                    var_bind=self.bbox_bind,
                    var_dest=self.bbox_dest,
                    sender=self.bbox_sender,
                    receiver=self.bbox_receiver,
                    inputs=self.get_inputs(False),
                    outputs=self.get_outputs(False)
                )

        raise FileNotFoundError("Blackbox boilerplate file not found")


if __name__ == "__main__":

    BASE_DIR = os.getcwd()
    DRVR_TEMPL_PATH = os.path.join(BASE_DIR, "template", "driver.tmp")
    BBOX_TEMPL_PATH = os.path.join(BASE_DIR, "template", "blackbox.tmp")

    galois = BoilerPlate(
        module="galois_lfsr",
        lib="proto",
        ipc="zmq",
        desc="Simple 4-bit Galois Linear Feedback Shift Register",
        drvr_templ_path=DRVR_TEMPL_PATH,
        bbox_templ_path=BBOX_TEMPL_PATH
    )
    galois.set_ports({
        "<bool> clock": "clock",
        "<bool> reset": "input",
        "<sc_uint<4> > data_out": "output",
    })
    print(galois.generate_sc_drvr())
    print(galois.generate_bbox())
