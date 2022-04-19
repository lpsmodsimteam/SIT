#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Implementation of the Verilog class

This class inherits from the BoilerPlate base class and implements its own methods of parsing,
modifying and generating boilerplate code for its specific paradigms.
"""

import os

from .boilerplate import BoilerPlate
from .exceptions import TemplateFileNotFound


class Verilog(BoilerPlate):
    def __init__(
        self,
        ipc,
        module,
        lib,
        module_dir="",
        lib_dir="",
        desc="",
        driver_template_path="",
        component_template_path="",
    ):
        """Constructor for Verilog BoilerPlate.

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
            component_template_path=component_template_path,
        )

        if self.ipc == "sock":

            # driver attributes
            self.driver_ipc = "socket"
            self.driver_bind = (
                "_sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)"
            )
            self.send = "sendall"
            self.connect = "connect"

        elif self.ipc == "zmq":

            # driver attributes
            self.driver_ipc = "zmq"
            self.driver_bind = """context = zmq.Context()
    _sock = context.socket(zmq.REQ)"""
            self.send = "send"
            self.connect = "bind"

        self.makefile_path = self.driver_path.replace(self.module, "Makefile.config")
        self.driver_path += "_driver.py"
        self.comp_path += "_comp.cpp"

    def _parse_signal_type(self, signal):
        """Parse the type and computes its size from the signal

        Parameters:
        -----------
        signal : str
            signal definition

        Returns:
        --------
        int
            signal width
        """

        # bit vector signals
        if "b" in signal:
            return self._get_ints(signal)

        elif signal == "1":
            return 1

        else:
            return self._get_num_digits(self._get_ints(signal))

    def _get_driver_outputs(self):
        """Generate output bindings for both the components in the black box

        Returns:
        --------
        str
            snippet of code representing output bindings
        """
        return self._sig_fmt(
            "str(dut.{sig}.value).encode()",
            lambda x: {"sig": x["name"]},
            self.ports["output"],
            " +\n" + " " * 8,
        )

    def _get_driver_inputs(self):
        """Generate input bindings for the driver.

        Returns:
        --------
        str
            snippet of code representing input bindings
        """
        fmt = "dut.{sig} = int(signal[{sp}:{sl}])"
        start_pos = 0
        clock_fmt = "dut.{sig} = int(signal[{sp}:{sl}]) % 2"
        driver_inputs = []

        for input_port in self.ports["input"]:
            driver_inputs.append(
                fmt.format(
                    sp=start_pos,
                    sl=str(input_port["len"] + start_pos),
                    sig=input_port["name"],
                )
            )
            start_pos += input_port["len"]

        if self.ports["clock"]:
            for clock_port in self.ports["clock"]:
                driver_inputs.append(
                    clock_fmt.format(
                        sp=start_pos,
                        sl=str(input_port["len"] + start_pos),
                        sig=clock_port["name"],
                    )
                )
                start_pos += int(clock_port["len"])

        self.driver_buf_size = start_pos + 1
        return ("\n" + " " * 8).join(driver_inputs)

    def _get_driver_defs(self):
        """Map definitions for the Verilog driver format string

        Returns:
        --------
        dict(str:str)
            format mapping of template Verilog driver string
        """
        return {
            "ipc": self.driver_ipc,
            "driver_bind": self.driver_bind,
            "connect": self.connect,
            "send": self.send,
            "module": self.module,
            "module_dir": self.module_dir,
            "buf_size": self.driver_buf_size,
        }

    def _generate_extra_files(self):

        template_str = ""
        makefile_templ_path = os.path.join(self.template_path, "Makefile.config")
        if os.path.isfile(makefile_templ_path):
            with open(makefile_templ_path) as template:
                template_str = template.read().format(
                    module=self.module, module_dir=os.path.abspath(self.module_dir)
                )
        else:
            raise TemplateFileNotFound(
                f"Component boilerplate template file: {makefile_templ_path} not found"
            )

        with open(self.makefile_path, "w") as makefile:
            makefile.write(template_str)
