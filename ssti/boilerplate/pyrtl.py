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

    def __init__(self, module, lib, ipc, drvr_templ_path="", comp_templ_path="",
                 desc="", module_dir="", lib_dir=""):
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
            module_dir=module_dir,
            lib_dir=lib_dir
        )

        if self.module_dir:
            self.module_dir = "sys.path.append(os.path.join(os.path.dirname(__file__), \"{}\"))".format(
                self.module_dir)

        if self.ipc == "sock":

            # driver attributes
            self.driver_ipc = "socket"
            self.driver_bind = """_sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)"""
            self.send = "sendall"

        elif self.ipc == "zmq":

            # driver attributes
            self.driver_ipc = "zmq"
            self.driver_bind = """context = zmq.Context()
_sock = context.socket(zmq.REQ)"""
            self.send = "send"

        self.driver_path += "_driver.py"
        self.comp_path += "_comp.cpp"

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

    def _get_driver_outputs(self):
        """Generates output bindings for both the components in the black box

        Returns:
            {str} -- snippet of code representing output bindings
        """
        return self.sig_fmt(
            "str({module}.sim.inspect({module}.{sig})).encode()",
            lambda x: {
                "module": self.module,
                "sig": x[-1]
            },
            self.outputs,
            " +\n" + " " * 8
        )

    def _get_driver_inputs(self):

        return self._get_inputs(
            fmt="\"{sig}\": int(signal[{sp}:{sl}]),",
            start_pos=0,
            signal_type_parser=self.__parse_signal_type,
            splice=True
        )

    def _get_driver_defs(self):

        return {
            "ipc": self.driver_ipc,
            "driver_bind": self.driver_bind,
            "send": self.send,
            "module_dir": self.module_dir,
            "module": self.module,
            "buf_size": self.buf_size
        }
