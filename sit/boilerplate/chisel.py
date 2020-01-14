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

        # if self.ipc == "sock":

        #     # driver attributes
        #     self.driver_ipc = "socket"
        #     self.driver_bind = "_sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)"
        #     self.send = "sendall"
        #     self.connect = "connect"

        self.driver_path += "_driver.scala"
        self.comp_path += "_comp.cpp"
        self.sbt_built = "build.sbt"

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
        if signal == "1":
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
            "peek(uut.io.{sig}).toString",
            lambda x: {
                "module": self.module,
                "sig": x[-1]
            },
            self.ports["output"],
            " +\n" + " " * 8
        )

    def _get_driver_inputs(self):
        """Generate input bindings for the driver.

        Returns:
        --------
        str
            snippet of code representing input bindings
        """
        fmt = "poke(uut.io.{sig}, signal.slice({sp}, {sl}).toInt{boolcmp})"
        start_pos = 0
        driver_inputs = []
        for input_type, input_name in self.ports["input"]:
            sig_len = self.__parse_signal_type(input_type)
            driver_inputs.append(
                fmt.format(
                    sp=start_pos,
                    sl=str(sig_len + start_pos),
                    sig=input_name,
                    boolcmp=((sig_len == 1) * " == 1")
                )
            )
            start_pos += sig_len

        self.buf_size = start_pos + 1
        return ("\n" + " " * 12).join(driver_inputs)

    def _get_driver_defs(self):
        """Map definitions for the Chisel driver format string

        Returns:
        --------
        dict(str:str)
            format mapping of template Chisel driver string
        """
        return {
            "module": self.module,
            "buf_size": self.buf_size
        }
