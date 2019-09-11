#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os


class BoilerPlate(object):

    def __init__(self, module, lib, ipc, drvr_templ_path, comp_templ_path,
                 desc="", link_desc=None,
                 module_dir="", lib_dir=""):
        """Constructor for BoilerPlate.

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
        if ipc in ("sock", "zmq"):
            self.ipc = ipc
        else:
            raise ValueError("Incorrect IPC protocol selected")

        self.module_dir = module_dir
        self.lib_dir = lib_dir
        self.module = module
        self.lib = lib
        self.drvr_templ_path = drvr_templ_path
        self.comp_templ_path = comp_templ_path
        self.desc = desc
        self.link_desc = link_desc if link_desc else {
            "link_desc0": "", "link_desc1": ""
        }

        self.clocks = []
        self.inputs = []
        self.outputs = []
        self.inouts = []
        self.ports = []

        if self.ipc == "sock":

            # component attributes
            self.comp_decl = """SocketSignal m_signal_io;"""
            self.comp_init = "m_signal_io(socket(AF_UNIX, SOCK_STREAM, 0)),"
            self.comp_bind = "m_signal_io.set_addr(m_ipc_port)"

        elif self.ipc == "zmq":

            # component attributes
            self.comp_decl = """zmq::context_t m_context;
    zmq::socket_t m_socket;
    ZMQSignal m_signal_io;"""
            self.comp_init = """m_context(1), m_socket(m_context, ZMQ_REP),
      m_signal_io(m_socket),"""
            self.comp_bind = "m_socket.bind(m_ipc_port.c_str())"

        # shared attributes
        self.sender = self.receiver = "m_signal_io"
        self.bbox_dir = "blackboxes"
        self.driver_path = self.comp_path = os.path.join(
            self.bbox_dir, self.module)
        self.WIDTH_DELIM = "//"

    @staticmethod
    def sig_fmt(fmt, split_func, array, delim=";\n    "):
        """Formats lists of signals based on fixed arguments

        Arguments:
            fmt {str} -- string format
            split_func {lambda/dict} -- map to split on the signals
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

        self.ports = self.clocks + self.inputs + self.outputs + self.inouts
        self.ports = [(i[0].split(self.WIDTH_DELIM)[0], i[-1])
                      for i in self.ports]

    def get_inputs(self, fmt, start_pos, signal_type_parser, splice=False, clock_fmt=""):
        """Generates input bindings for both the components in the black box

        Arguments:
            driver {bool} -- option to generate code for the black box-driver
                (default: {True})

        Returns:
            {str} -- snippet of code representing input bindings
        """
        driver_inputs = []
        for driver_input in self.inputs:
            sig_len = signal_type_parser(driver_input[0])
            driver_inputs.append(
                fmt.format(
                    sp=start_pos,
                    sl=str(sig_len + (splice * start_pos)),
                    sig=driver_input[-1],
                )
            )
            start_pos += sig_len

        if self.clocks:
            driver_inputs.append(
                clock_fmt.format(
                    sp=start_pos,
                    sig=self.clocks[-1][-1],
                )
            )

        driver_inputs = ("\n" + " " * 8).join(driver_inputs)

        return {
            "inputs": driver_inputs,
            "sig_len": start_pos + 1
        } if splice else driver_inputs

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
                    sender=self.sender,
                    receiver=self.receiver,
                )

        raise FileNotFoundError("Component boilerplate file not found")

    def generate_bbox(self):
        """Provides a high-level interface to the user to generate both the
        components of the black box and dump them to their corresponding files
        """
        if not len(self.ports):
            raise IndexError("Ports were not set properly")

        if not os.path.exists(self.bbox_dir):
            os.makedirs(self.bbox_dir)

        if self.driver_path:
            with open(self.driver_path, "w") as driver_file:
                driver_file.write(self.generate_driver())

        if self.comp_path:
            with open(self.comp_path, "w") as comp_file:
                comp_file.write(self.generate_comp())
