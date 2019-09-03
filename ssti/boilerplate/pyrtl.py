#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Implementation of the BoilerPlate class

This class generates the boilerplate code required to build the black box
interface in SSTI.
"""

import math
import os

from .boilerplate import BoilerPlate


class PyRTL(BoilerPlate):

    def __init__(self, module, lib, ipc,
                 drvr_templ_path="", comp_templ_path="",
                 desc="", link_desc=None, module_dir="", lib_dir=""):
        """Constructor for PyRTL BoilerPlate.

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
            os.path.dirname(__file__), "template", "pyrtl")
        if not drvr_templ_path:
            drvr_templ_path = os.path.join(templ_path, "driver.py")
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
        print(self.drvr_templ_path)

        self.start_pos = 0

        if self.ipc == "sock":
            self.driver_ipc = "socket"
            self.driver_bind = """_sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)"""
            self.send = "sendall"
            self.sender = self.receiver = "m_signal_io"

            self.comp_decl = """SocketSignal m_signal_io;"""
            self.comp_init = "m_signal_io(0, socket(AF_UNIX, SOCK_STREAM, 0)),"
            self.comp_bind = "m_signal_io.set_addr(m_ipc_port)"
            self.comp_dest = ""

        elif self.ipc == "zmq":
            self.driver_ipc = "zmq"
            self.driver_bind = """context = zmq.Context()
_sock = context.socket(zmq.REQ)"""
            self.send = "send"

            self.comp_decl = """zmq::context_t m_context;
    zmq::socket_t m_socket;
    ZMQReceiver m_signal_i;
    ZMQTransmitter m_signal_o;"""
            self.comp_init = """m_context(1), m_socket(m_context, ZMQ_REP),
      m_signal_i(0, m_socket), m_signal_o(0, m_socket),"""
            self.comp_bind = "m_socket.bind(m_ipc_port.c_str())"
            self.comp_dest = "m_socket.close();"
            self.sender = "m_signal_o"
            self.receiver = "m_signal_i"

        self.driver_path += "_driver.py"
        self.comp_path += "_comp.cpp"
        self.ports_path = None

    @staticmethod
    def __parse_signal_type(signal):
        """Parses the type and computes its size from the signal

        Arguments:
            signal {str} -- signal definition

        Returns:
            {tuple(str,int)} -- C++ datatype and its size
        """
        # NoneTypes are explicitly assigned to SST component clock signals
        if signal == "1":
            return 1

        def __get_ints(sig):
            return int("".join(s for s in sig if s.isdigit()))

        return math.floor(math.log2(__get_ints(signal)))

    def get_inputs(self):
        """Generates input bindings for both the components in the black box

        Arguments:
            driver {bool} -- option to generate code for the black box-driver
                (default: {True})

        Returns:
            {str} -- snippet of code representing input bindings
        """
        inputs = []
        fmt = "\"{sig}\": int(signal[{sp}:{sl}]),"
        for driver_input in self.inputs:
            sig_len = self.__parse_signal_type(driver_input[0])
            inputs.append(
                fmt.format(
                    sp=self.start_pos,
                    sl=str(sig_len + self.start_pos),
                    sig=driver_input[-1],
                )
            )
            self.start_pos += sig_len

        return ("\n" + " " * 8).join(inputs)

    def get_outputs(self):
        """Generates output bindings for both the components in the black box

        Arguments:
            driver {bool} -- option to generate code for the black box-driver
                (default: {True})

        Returns:
            {str} -- snippet of code representing output bindings
        """
        return self.sig_fmt(
            "str({module}.sim.inspect({module}.{sig})).encode()",
            lambda x: {
                "module": self.module,
                "sig": x[-1]
            }, self.outputs,
            " +\n" + " " * 8
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
                    ipc=self.driver_ipc,
                    driver_bind=self.driver_bind,
                    send=self.send,
                    module_dir=self.module_dir,
                    module=self.module,
                    inputs=self.get_inputs(),
                    sig_len=self.start_pos + 1 if self.driver_ipc == "socket" else "",
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
