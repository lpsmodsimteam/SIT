#!/usr/bin/env python
# -*- coding: utf-8 -*-

import math
import os
from string import punctuation


class BoilerPlate(object):

    def __init__(self, module, lib, ipc, drvr_templ_path, sst_model_templ_path,
                 desc="", link_desc=None,
                 module_dir="", ports_dir="", lib_dir=""):
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

        self.module_dir = module_dir
        self.ports_dir = ports_dir
        self.lib_dir = lib_dir
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

        self.WIDTH_DELIM = "//"
        self.bbox_dir = "blackboxes"

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
