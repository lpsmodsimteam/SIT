#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Implementation of the BoilerPlate class

This class generates the boilerplate code required to build the black box
interface in SSTSCIT.
"""

import math
import os
from string import punctuation


class BoilerPlate(object):

    def __init__(self, module, lib, ipc, drvr_templ_path, sst_model_templ_path,
                 desc="", link_desc=None):
        """Constructor for BoilerPlate.

        Arguments:
            module {str} -- module name
            lib {str} -- SST library name
            ipc {str} -- type of IPC. Supported options are ("sock", "zmq")
            drvr_templ_path {str} -- path to the black box-driver boilerplate
            sst_model_templ_path {str} -- path to the black box-model boilerplate
            desc {str} -- description of the SST model (default: {""})
            link_desc {dict(str:str)} -- description of the SST links
                The argument defaults to:
                `{"link_desc0": "", "link_desc1": ""}`
                where they're assigned as the receiving and transmitting SST
                links respectively.
        """
        if ipc in ("sock", "zmq"):
            self.ipc = ipc
        else:
            raise ValueError("Incorrect IPC protocol selected")

        self.module = module
        self.lib = lib
        self.drvr_templ_path = drvr_templ_path
        self.sst_model_templ_path = sst_model_templ_path
        self.desc = desc
        self.link_desc = link_desc if link_desc else {
            "link_desc0": "", "link_desc1": ""
        }

        self.clocks = []
        self.inputs = []
        self.outputs = []
        self.inouts = []
        self.ports = []
        self.abbr = "".join(
            i for i in self.module if i not in punctuation + "aeiou")

        self.enums = """int {abbr}_NPORTS = {nports};

enum {module}_ports {{
    __pid__, {ports}
}};
"""

        if self.ipc in ("sock", "socks", "socket", "sockets"):
            self.driver_decl = """// Initialize signal handlers
    SocketSignal m_signal_io({}_NPORTS, socket(AF_UNIX, SOCK_STREAM, 0), false);
    m_signal_io.set_addr(argv[1]);""".format(
                self.abbr.upper()
            )
            self.drvr_dest = ""
            self.sender = self.receiver = "m_signal_io"

            self.sst_model_decl = """SocketSignal m_signal_io;"""
            self.sst_model_init = """m_signal_io({}_NPORTS, socket(AF_UNIX, SOCK_STREAM, 0)),""".format(
                self.abbr.upper()
            )
            self.sst_model_bind = "m_signal_io.set_addr(m_ipc_port)"
            self.sst_model_dest = ""

        elif self.ipc == "zmq":
            self.driver_decl = """// Socket to talk to server
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect(argv[1]);

    // Initialize signal handlers
    ZMQReceiver m_signal_i({n}_NPORTS, socket);
    ZMQTransmitter m_signal_o({n}_NPORTS, socket);""".format(
                n=self.abbr.upper()
            )
            self.drvr_dest = "socket.close();"
            self.sst_model_decl = """zmq::context_t m_context;
    zmq::socket_t m_socket;
    ZMQReceiver m_signal_i;
    ZMQTransmitter m_signal_o;"""
            self.sst_model_init = """m_context(1), m_socket(m_context, ZMQ_REP),
      m_signal_i({n}_NPORTS, m_socket), m_signal_o({n}_NPORTS, m_socket),""".format(
                n=self.abbr.upper()
            )
            self.sst_model_bind = "m_socket.bind(m_ipc_port.c_str())"
            self.sst_model_dest = "m_socket.close();"
            self.sender = "m_signal_o"
            self.receiver = "m_signal_i"

        self.bbox_dir = "blackboxes"
        self.sc_driver_path = self.bbox_dir + "/" + self.module + "_driver.cpp"
        self.sst_model_path = self.bbox_dir + "/" + self.module + "_comp.cpp"
        self.sst_ports_path = self.bbox_dir + "/" + self.module + "_ports.hpp"

    @staticmethod
    def __parse_signal_type(signal):
        """Parses the type and computes its size from the signal

        Arguments:
            signal {str} -- signal definition

        Returns:
            {tuple(str,int)} -- C++ datatype and its size
        """
        # NoneTypes are explicitly assigned to SST component clock signals
        if not signal:
            return signal, 0

        elif "sc" in signal:

            def __get_ints(sig):
                return int("".join(s for s in sig if s.isdigit()))

            if "int" in signal:
                return "<int>", math.floor(math.log2(__get_ints(signal)))

            if "bv" in signal or "lv" in signal:
                return "<int>", __get_ints(signal)

            if "bit" in signal or "logic" in signal:
                return "<bool>", __get_ints(signal)

        else:
            return signal, 1

    @staticmethod
    def __format(fmt, split_func, array, delim=";\n    "):
        """Formats lists of signals based on fixed arguments

        Arguments:
            fmt {str} -- string format
            split_func {func} -- lambda function to split on the signals
            array {list(str)} -- list of signals
            delim {str} -- delimiter (default: {";n    "})

        Returns:
            {str} -- string formatted signals
        """
        return delim.join(fmt.format(**split_func(i)) for i in array)

    def set_ports(self, ports):
        """Assigns ports to their corresponding member lists

        Arguments:
            ports {tuple(tuple(str,str,str),)} -- tuple of C++-style
                type-declared signals in the form
                ("<DTYPE>", "<PORT NAME>", "<PORT TYPE>"). The current types of
                signals supported are ("clock", "input", "output", "inout")
        """
        for port_dtype, port_name, port_type in ports:
            if port_type == "clock":
                self.clocks.append((port_dtype, port_name))
            elif port_type == "input":
                self.inputs.append((port_dtype, port_name))
            elif port_type == "output":
                self.outputs.append((port_dtype, port_name))
            elif port_type == "inout":
                self.inouts.append((port_dtype, port_name))
            else:
                raise ValueError("Each ports must be designated a type")
            self.ports.append((port_dtype, port_name))

    def get_driver_port_defs(self):
        """Generates port definitions for the black box-driver"""
        return "sc_signal" + ";\n    sc_signal".join(
            " ".join(i) for i in self.ports)

    def get_driver_bindings(self):
        """Generates port bindings for the black box-driver

        Returns:
            {str} -- snippet of code representing port bindings
        """
        return self.__format(
            "DUT.{sig}({sig})",
            lambda x: {"sig": x[-1]}, self.ports
        )

    def get_clock(self, driver=True):
        """Generates clock binding for both the components in the black box

        Arguments:
            driver {bool} -- option to generate code for the black box-driver
                (default: {True})

        Returns:
            {str} -- snippet of code representing clock binding
        """
        return self.__format(
            "{sig} = {recv}.get_clock_pulse({module}_ports::{abbr}_{sig})",
            lambda x: {
                "abbr": self.abbr,
                "module": self.module,
                "recv": self.receiver,
                "sig": x[-1]
            }, self.clocks
        ) if driver else [[None, i[-1]] for i in self.clocks]

    def get_inputs(self, driver=True):
        """Generates input bindings for both the components in the black box

        Arguments:
            driver {bool} -- option to generate code for the black box-driver
                (default: {True})

        Returns:
            {str} -- snippet of code representing input bindings
        """
        return self.__format(
            "{sig} = {recv}.get{type}({module}_ports::{abbr}_{sig})",
            lambda x: {
                "abbr": self.abbr,
                "module": self.module,
                "recv": self.receiver,
                "sig": x[-1],
                "type": self.__parse_signal_type(x[0])[0],
            }, self.inputs, ";\n" + " " * 8
        ) if driver else self.__format(
            "std::to_string({recv}.get{type}({module}_ports::{abbr}_{sig}))",
            lambda x: {
                "abbr": self.abbr,
                "module": self.module,
                "recv": self.receiver,
                "sig": x[-1],
                "type": self.__parse_signal_type(x[0])[0],
            }, self.outputs, " +\n" + " " * 16
        )

    def get_outputs(self, driver=True):
        """Generates output bindings for both the components in the black box

        Arguments:
            driver {bool} -- option to generate code for the black box-driver
                (default: {True})

        Returns:
            {str} -- snippet of code representing output bindings
        """
        if driver:

            return self.__format(
                "{send}.set({module}_ports::{abbr}_{sig}, {sig})",
                lambda x: {
                    "abbr": self.abbr,
                    "module": self.module,
                    "send": self.sender,
                    "sig": x[-1],
                }, self.outputs, ";\n" + " " * 8
            )

        # else, the SST component requires evaluation of signal type lengths
        # positions 0 and 1 are reserved for stopping sending and receiving of
        # signals
        start_pos = 2
        sst_model_inputs = self.inputs + self.get_clock(driver=False)
        sst_model_output = []
        fmt = "{send}.set({module}_ports::{abbr}_{sig}, std::stoi(_data_in.substr({sp}{sl})))"
        for model_input in sst_model_inputs:
            sig_len = self.__parse_signal_type(model_input[0])[-1]
            sst_model_output.append(
                fmt.format(
                    abbr=self.abbr,
                    sl=(", " + str(sig_len)) * bool(sig_len),
                    module=self.module,
                    sp=start_pos,
                    send=self.sender,
                    sig=model_input[-1],
                )
            )
            start_pos += sig_len

        return (";\n" + " " * 8).join(sst_model_output)

    def generate_sc_driver(self):
        """Generates the black box-driver code based on methods used to format
        the template file

        Returns:
            {str} -- boilerplate code representing the black box-driver file
        """
        if os.path.isfile(self.drvr_templ_path):
            with open(self.drvr_templ_path) as template:
                return template.read().format(
                    module=self.module,
                    port_defs=self.get_driver_port_defs(),
                    bindings=self.get_driver_bindings(),
                    var_decl=self.driver_decl,
                    dest=self.drvr_dest,
                    clock=self.get_clock(),
                    sender=self.sender,
                    receiver=self.receiver,
                    inputs=self.get_inputs(),
                    outputs=self.get_outputs()
                )

        raise FileNotFoundError("Driver boilerplate file not found")

    def generate_sst_model(self):
        """Generates the black box-model code based on methods used to format
        the template file

        Returns:
            {str} -- boilerplate code representing the black box-model file
        """
        if os.path.isfile(self.sst_model_templ_path):
            with open(self.sst_model_templ_path) as template:
                return template.read().format(
                    module=self.module,
                    lib=self.lib,
                    comp=self.module,
                    desc=self.desc,
                    **self.link_desc,
                    var_decl=self.sst_model_decl,
                    var_init=self.sst_model_init,
                    var_bind=self.sst_model_bind,
                    var_dest=self.sst_model_dest,
                    sender=self.sender,
                    receiver=self.receiver,
                    inputs=self.get_inputs(False),
                    outputs=self.get_outputs(False),
                )

        raise FileNotFoundError("Model boilerplate file not found")

    def generate_ports_enum(self):
        """Generates the number of ports and the port enumeration for the black
        box components

        Returns:
            {str} -- boilerplate code representing the black box port details
        """
        ports = [self.abbr + "_" + x[-1] for x in self.ports]
        return self.enums.format(
            abbr=self.abbr.upper(),
            nports=len(ports) + 1,
            module=self.module,
            ports=", ".join(ports)
        )

    def generate_bbox(self):
        """Provides a high-level interface to the user to generate both the
        components of the black box and dump them to their corresponding files
        """
        if not len(self.ports):
            raise IndexError("Ports were not set properly")

        if not os.path.exists(self.bbox_dir):
            os.makedirs(self.bbox_dir)

        with open(self.sc_driver_path, "w") as sc_driver_file:
            sc_driver_file.write(self.generate_sc_driver())

        with open(self.sst_model_path, "w") as sst_model_file:
            sst_model_file.write(self.generate_sst_model())

        with open(self.sst_ports_path, "w") as sst_ports_file:
            sst_ports_file.write(self.generate_ports_enum())
