#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Implementation of the BoilerPlate class

This class generates the boilerplate code required to build the black box
interface in SIT.
"""

import math

from .boilerplate import BoilerPlate


class PyRTL(BoilerPlate):

    def __init__(self, ipc, module, lib, module_dir="", lib_dir="", desc="",
                 driver_template_path="", component_template_path=""):
        """Constructor for PyRTL BoilerPlate.

        Parameters:
        -----------
        ipc : str (options: "sock", "zmq")
            method of IPC
        module : str
            SST element component and HDL module name
        lib : str
            SST element library name
        module_dir : str (default: "")
            directory of HDL module
        lib_dir : str (default: "")
            directory of SIT library
        desc : str (default: "")
            description of the SST model
        driver_template_path : str (default: "")
            path to the black box-driver boilerplate
        component_template_path : str (default: "")
            path to the black box-model boilerplate
        """
        super().__init__(
            ipc=ipc,
            module=module,
            lib=lib,
            module_dir=module_dir,
            lib_dir=lib_dir,
            desc=desc,
            driver_template_path=driver_template_path,
            component_template_path=component_template_path
        )

        if self.module_dir:
            self.module_dir = f"sys.path.append(os.path.join(os.path.dirname(__file__), \"{self.module_dir}\"))"

        if self.ipc == "sock":

            # driver attributes
            self.driver_ipc = "socket"
            self.driver_bind = """_sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)"""
            self.send = "sendall"
            self.connect = "connect"

        elif self.ipc == "zmq":

            # driver attributes
            self.driver_ipc = "zmq"
            self.driver_bind = """context = zmq.Context()
_sock = context.socket(zmq.REQ)"""
            self.send = "send"
            self.connect = "bind"

        self.driver_path += "_driver.py"
        self.comp_path += "_comp.cpp"

    @staticmethod
    def __parse_signal_type(signal):
        """Parses the type and computes its size from the signal

        Parameters:
        -----------
        signal : str
            signal definition

        Returns:
        --------
        int
            signal size
        """
        if signal == "1":
            return 1

        def __get_ints(sig):
            return int("".join(s for s in sig if s.isdigit()))

        return math.floor(math.log2(__get_ints(signal)))

    def _get_driver_outputs(self):
        """Generates output bindings for both the components in the black box

        Returns:
        --------
        str
            snippet of code representing output bindings
        """
        return self._sig_fmt(
            "str({module}.sim.inspect({module}.{sig})).encode()",
            lambda x: {
                "module": self.module,
                "sig": x[-1]
            },
            self.outputs,
            " +\n" + " " * 8
        )

    def _get_driver_inputs(self):

        return self._generate_driver_inputs(
            fmt="\"{sig}\": int(signal[{sp}:{sl}]),",
            start_pos=0,
            signal_type_parser=self.__parse_signal_type,
            splice=True
        )

    def _get_driver_defs(self):
        """Map definitions for the PyRTL driver format string

        Returns:
        --------
        dict(str:str)
            format mapping of template PyRTL driver string
        """
        return {
            "ipc": self.driver_ipc,
            "driver_bind": self.driver_bind,
            "connect": self.connect,
            "send": self.send,
            "module_dir": self.module_dir,
            "module": self.module,
            "buf_size": self.buf_size
        }
