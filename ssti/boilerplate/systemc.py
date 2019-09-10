#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Implementation of the PyRTL class

This class generates the boilerplate code required to build the black box
interface in SSTI.
"""

import math
import os

from .boilerplate import BoilerPlate


class SystemC(BoilerPlate):

    def __init__(self, module, lib, ipc, drvr_templ_path="", comp_templ_path="",
                 desc="", link_desc=None, module_dir="", lib_dir=""):
        """Constructor for SystemC BoilerPlate.

        Arguments:
            module {str} -- module name
            lib {str} -- SST library name
            ipc {str} -- type of IPC. Supported options are ("sock", "zmq")
            drvr_templ_path {str} -- path to the black box-driver boilerplate
            comp_templ_path {str} -- path to the black box-model boilerplate
            desc {str} -- description of the SST model (default: {""})
            link_desc {dict(str:str)} -- description of the SST links
                The argument defaults to:
                `{"link_desc0": "", "link_desc1": ""}`
                where they're assigned as the receiving and transmitting SST
                links respectively.
        """
        templ_path = os.path.join(
            os.path.dirname(__file__), "template", "systemc")
        if not drvr_templ_path:
            drvr_templ_path = os.path.join(templ_path, "driver.cpp")
        if not comp_templ_path:
            comp_templ_path = os.path.join(templ_path, "comp.cpp")

        super().__init__(
            module=module,
            lib=lib,
            ipc=ipc,
            drvr_templ_path=drvr_templ_path,
            comp_templ_path=comp_templ_path,
            desc=desc,
            link_desc=link_desc,
            module_dir=module_dir,
            lib_dir=lib_dir
        )

        if self.ipc == "sock":

            # driver attributes
            self.driver_decl = """// Initialize signal handlers
    SocketSignal m_signal_io(socket(AF_UNIX, SOCK_STREAM, 0), false);
    m_signal_io.set_addr(argv[1]);"""
            self.drvr_dest = ""

            # component attributes
            self.comp_decl = """SocketSignal m_signal_io;"""
            self.comp_init = """m_signal_io(socket(AF_UNIX, SOCK_STREAM, 0)),"""
            self.comp_bind = "m_signal_io.set_addr(m_ipc_port)"
            self.comp_dest = ""

            # shared attributes
            self.sender = self.receiver = "m_signal_io"

        elif self.ipc == "zmq":

            # driver attributes
            self.driver_decl = """// Socket to talk to server
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect(argv[1]);

    // Initialize signal handlers
    ZMQReceiver m_signal_i(socket);
    ZMQTransmitter m_signal_o(socket);"""
            self.drvr_dest = "socket.close();"

            # component attributes
            self.comp_decl = """zmq::context_t m_context;
    zmq::socket_t m_socket;
    ZMQReceiver m_signal_i;
    ZMQTransmitter m_signal_o;"""
            self.comp_init = """m_context(1), m_socket(m_context, ZMQ_REP),
      m_signal_i(m_socket), m_signal_o(m_socket),"""
            self.comp_bind = "m_socket.bind(m_ipc_port.c_str())"
            self.comp_dest = "m_socket.close();"

            # shared attributes
            self.sender = "m_signal_o"
            self.receiver = "m_signal_i"

        self.driver_path += "_driver.cpp"
        self.comp_path += "_comp.cpp"

    def __parse_signal_type(self, signal):
        """Parses the type and computes its size from the signal

        Arguments:
            signal {str} -- signal definition

        Returns:
            {tuple(str,int)} -- C++ datatype and its size
        """
        # NoneTypes are explicitly assigned to SST component clock signals
        if not signal:
            return 0

        elif "sc" in signal:

            def __get_ints(sig):
                return int("".join(s for s in sig if s.isdigit())
                           if self.WIDTH_DELIM not in sig
                           else sig.split(self.WIDTH_DELIM)[-1])

            if "bv" in signal or "lv" in signal or "int" in signal:
                return math.floor(math.log2(__get_ints(signal)))

            if "bit" in signal or "logic" in signal:
                return __get_ints(signal)

        else:
            return 1

    def get_outputs(self):
        """Generates output bindings for both the components in the black box

        Returns:
            {str} -- snippet of code representing output bindings
        """
        return self.sig_fmt(
            "ss << {sig}",
            lambda x: {
                "sig": x[-1],
            },
            self.outputs,
            ";\n" + " " * 8
        )

    def get_driver_port_defs(self):
        """Generates port definitions for the black box-driver"""
        return "sc_signal" + ";\n    sc_signal".join(
            " ".join(i) for i in self.ports)

    def get_driver_bindings(self):
        """Generates port bindings for the black box-driver

        Returns:
            {str} -- snippet of code representing port bindings
        """
        return self.sig_fmt(
            "DUT.{sig}({sig})",
            lambda x: {"sig": x[-1]}, self.ports
        )

    def generate_driver(self):
        """Generates the black box-driver code based on methods used to format
        the template file

        Returns:
            {str} -- boilerplate code representing the black box-driver file
        """
        if os.path.isfile(self.drvr_templ_path):
            with open(self.drvr_templ_path) as template:
                return template.read().format(
                    module_dir=self.module_dir,
                    lib_dir=self.lib_dir,
                    module=self.module,
                    port_defs=self.get_driver_port_defs(),
                    bindings=self.get_driver_bindings(),
                    var_decl=self.driver_decl,
                    dest=self.drvr_dest,
                    sender=self.sender,
                    receiver=self.receiver,
                    inputs=self.get_inputs(
                        fmt="{sig} = std::stoi(_data_in.substr({sp}, {sl}));",
                        start_pos=1,
                        signal_type_parser=self.__parse_signal_type
                    ),
                    outputs=self.get_outputs()
                )

        raise FileNotFoundError("Driver boilerplate file not found")

    def generate_comp(self):
        """Generates the black box-model code based on methods used to format
        the template file

        Returns:
            {str} -- boilerplate code representing the black box-model file
        """
        if os.path.isfile(self.comp_templ_path):
            with open(self.comp_templ_path) as template:
                return template.read().format(
                    lib_dir=self.lib_dir,
                    module=self.module,
                    lib=self.lib,
                    comp=self.module,
                    desc=self.desc,
                    **self.link_desc,
                    var_decl=self.comp_decl,
                    var_init=self.comp_init,
                    var_bind=self.comp_bind,
                    var_dest=self.comp_dest,
                    sender=self.sender,
                    receiver=self.receiver,
                )

        raise FileNotFoundError("Model boilerplate file not found")
