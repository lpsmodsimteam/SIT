#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os


class BoilerPlate():

    def __init__(self, module, lib, ipc, drvr_templ_path, bbox_templ_path,
                 desc="", link_desc={"link_desc0": "", "link_desc1": ""}):

        if ipc in ("sock", "socks", "socket", "sockets", "zmq"):
            self.ipc = ipc
        else:
            raise ValueError("Incorrect IPC protocol selected")

        self.module = module
        self.lib = lib
        self.drvr_templ_path = drvr_templ_path
        self.bbox_templ_path = bbox_templ_path
        self.desc = desc
        self.link_desc = link_desc

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

        if self.ipc in ("sock", "socks", "socket", "sockets"):
            self.drvr_decl = """// Initialize signal handlers
    SignalSocket m_signal_io(socket(AF_UNIX, SOCK_STREAM, 0), false);
    m_signal_io.set_addr(argv[1]);"""
            self.drvr_dest = ""
            self.sender = self.receiver = "m_signal_io"

            self.bbox_decl = """SignalSocket m_signal_io;"""
            self.bbox_init = """m_signal_io(socket(AF_UNIX, SOCK_STREAM, 0)),"""
            self.bbox_bind = "m_signal_io.set_addr(m_ipc_port)"
            self.bbox_dest = ""

        elif self.ipc == "zmq":
            self.drvr_decl = """// Socket to talk to server
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect(argv[1]);

    // Initialize signal handlers
    ZMQReceiver m_signal_i(socket);
    ZMQTransmitter m_signal_o(socket);"""
            self.drvr_dest = "socket.close();"
            self.bbox_decl = """zmq::context_t m_context;
    zmq::socket_t m_socket;
    ZMQReceiver m_signal_i;
    ZMQTransmitter m_signal_o;"""
            self.bbox_init = """m_context(1), m_socket(m_context, ZMQ_REP),
      m_signal_i(m_socket), m_signal_o(m_socket),"""
            self.bbox_bind = "m_socket.bind(m_ipc_port.c_str())"
            self.bbox_dest = "m_socket.close();"
            self.sender = "m_signal_o"
            self.receiver = "m_signal_i"

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

    def get_port_defs(self):

        return ";\n    ".join(self.ports)

    def get_bindings(self):

        return self.__format(
            "DUT.{sig}({sig})",
            lambda x: {"sig": x.split(" ")[-1]}, self.ports
        )

    def get_clock(self, drvr=True):

        return self.__format(
            "{sig} = {recv}.get_clock_pulse(\"{sig}\")",
            lambda x: {"sig": x[-1], "recv": self.receiver}, self.clocks
        ) if drvr else [["<sc_int<2>>", i[-1]] for i in self.clocks]

    def get_inputs(self, drvr=True):

        return self.__format(
            "{sig} = {recv}.get{type}(\"{sig}\")",
            lambda x: {
                "sig": x[-1], "type": self.__parse_signal_type(x[0])[0],
                "recv": self.receiver
            }, self.inputs, ";\n" + " " * 8
        ) if drvr else self.__format(
            "std::to_string({recv}.get{type}(\"{sig}\"))",
            lambda x: {
                "sig": x.split(" ")[-1],
                "type": self.__parse_signal_type(x.split(" ")[0])[0],
                "recv": self.receiver
            }, self.outputs, " +\n" + " " * 16
        )

    def get_outputs(self, drvr=True):

        if drvr:

            return self.__format(
                "{send}.set(\"{sig}\", {sig})",
                lambda x: {"sig": x.split(" ")[-1],
                           "send": self.sender},
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
            "{send}.set(\"{sig}\", std::stoi(_data_in.substr({p}, {l})))",
            lambda x: {
                "sig": x[-1],
                "p": sig_lens[bbox_inputs.index(x)][0],
                "l": sig_lens[bbox_inputs.index(x)][-1],
                "send": self.sender
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
                    dest=self.drvr_dest,
                    clock=self.get_clock(),
                    sender=self.sender,
                    receiver=self.receiver,
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
                    **self.link_desc,
                    var_decl=self.bbox_decl,
                    var_init=self.bbox_init,
                    var_bind=self.bbox_bind,
                    var_dest=self.bbox_dest,
                    sender=self.sender,
                    receiver=self.receiver,
                    inputs=self.get_inputs(False),
                    outputs=self.get_outputs(False),
                )

        raise FileNotFoundError("Blackbox boilerplate file not found")
