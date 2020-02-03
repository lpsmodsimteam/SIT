#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Implementation of the Chisel class

This class inherits from the BoilerPlate base class and implements its own methods of parsing,
modifying and generating boilerplate code for its specific paradigms.
"""

import math
import os

from .boilerplate import BoilerPlate
from .exceptions import IPCException, TemplateFileNotFound


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

        if self.ipc != "sock":
            raise IPCException(f"Java does not support {self.ipc}")

        self.sbt_build_path = self.driver_path.replace(self.module, "build.sbt")
        self.driver_path += "_driver.scala"
        self.comp_path += "_comp.cpp"

    @staticmethod
    def _parse_signal_type(signal):
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
                "sig": x["name"]
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

        # input_port = (INPUT NAME, INPUT TYPE)
        for input_port in self.ports["input"]:
            driver_inputs.append(
                fmt.format(
                    sp=start_pos,
                    sl=str(input_port["len"] + start_pos),
                    sig=input_port["name"],
                    boolcmp=((input_port["len"] == 1) * " == 1")
                )
            )
            start_pos += input_port["len"]

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

    def _generate_extra_files(self):

        template_str = ""
        build_sbt_templ_path = os.path.join(self.template_path, "build.sbt")
        if os.path.isfile(build_sbt_templ_path):
            with open(build_sbt_templ_path) as template:
                template_str = template.read().format(
                    module=self.module
                )
        else:
            raise TemplateFileNotFound(
                f"Component boilerplate template file: {build_sbt_templ_path} not found")

        with open(self.sbt_build_path, "w") as build_file:
            build_file.write(template_str)
