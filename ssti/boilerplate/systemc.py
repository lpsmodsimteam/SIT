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
                 desc="", module_dir="", lib_dir=""):
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
            module_dir=module_dir,
            lib_dir=lib_dir
        )

        if self.ipc == "sock":

            # driver attributes
            self.sig_type = "SocketSignal"

        elif self.ipc == "zmq":

            # driver attributes
            self.sig_type = "ZMQSignal"

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

    def _get_driver_outputs(self):
        """Generates output bindings for both the components in the black box

        Returns:
            {str} -- snippet of code representing output bindings
        """
        return self.sig_fmt(
            "_data_out << {sig}",
            lambda x: {
                "sig": x[-1],
            },
            self.outputs,
            ";\n" + " " * 8
        )

    def _get_driver_inputs(self):

        return self._get_inputs(
            fmt="{sig} = std::stoi(_data_in.substr({sp}, {sl}));",
            start_pos=1,
            signal_type_parser=self.__parse_signal_type,
            clock_fmt="{sig} = std::stoi(_data_in.substr({sp})) % 2;",
        )

    def __get_driver_port_defs(self):
        """Generates port definitions for the black box-driver"""
        return "sc_signal" + ";\n    sc_signal".join(
            " ".join(i) for i in self.ports)

    def __get_driver_bindings(self):
        """Generates port bindings for the black box-driver

        Returns:
            {str} -- snippet of code representing port bindings
        """
        return self.sig_fmt(
            "DUT.{sig}({sig})",
            lambda x: {"sig": x[-1]}, self.ports
        )

    def _get_driver_defs(self):

        return {
            "module_dir": self.module_dir,
            "lib_dir": self.lib_dir,
            "module": self.module,
            "port_defs": self.__get_driver_port_defs(),
            "bindings": self.__get_driver_bindings(),
            "sender": self.sender,
            "receiver": self.receiver,
            "buf_size": self.buf_size,
            "sig_type": self.sig_type,
        }