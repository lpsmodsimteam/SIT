#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Implementation of the Chisel class

This class inherits from the BoilerPlate base class and implements its own methods of parsing,
modifying and generating boilerplate code for its specific paradigms.
"""

import math

from .boilerplate import BoilerPlate


class Chisel(BoilerPlate):

    def __init__(self, ipc, module, lib, module_dir="", lib_dir="", desc="",
                 driver_template_path="", component_template_path=""):
        """Constructor for Chisel BoilerPlate.

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

        if self.ipc == "sock":

            # driver attributes
            self.driver_ipc = "socket"
            self.driver_bind = """_sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)"""
            self.send = "sendall"
            self.connect = "connect"

        self.driver_path += "_driver.scala"
        self.comp_path += "_comp.cpp"

    @staticmethod
    def __parse_signal_type(signal):
        """Parse the type and computes its width from the signal

        Parameters:
        -----------
        signal : str
            signal definition

        Returns:
        --------
        int
            signal width
        """
        if any(x in signal for x in ("bool", "Bool", "Boolean")):
            return 1

        def __get_ints(sig):
            return int("".join(s for s in sig if s.isdigit()))

        return math.floor(math.log2(__get_ints(signal)))

    def _get_driver_outputs(self):
        """Generate output bindings for both the components in the black box

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
            self.ports["output"],
            " +\n" + " " * 8
        )

    def _get_driver_inputs(self):
        """Wrap base _generate_driver_inputs() method with overridden parameters

        Returns:
        --------
        func
            _generate_driver_inputs(*args)
        """
        return self._generate_driver_inputs(
            fmt="var {sig} = signal.slice({sp}, {sl}).toInt",
            start_pos=0,
            signal_type_parser=self.__parse_signal_type,
            splice=True
        )

    def _get_driver_defs(self):
        """Map definitions for the Chisel driver format string

        Returns:
        --------
        dict(str:str)
            format mapping of template Chisel driver string
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
