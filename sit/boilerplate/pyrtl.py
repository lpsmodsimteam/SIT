#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Implementation of the PyRTL class

This class inherits from the BoilerPlate base class and implements its own methods of parsing,
modifying and generating boilerplate code for its specific paradigms.
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
            self.driver_bind = "_sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)"
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

        return math.floor(math.log(math.pow(2, __get_ints(signal)) - 1) / math.log(10)) + 1

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
        fmt = "\"{sig}\": int(signal[{sp}:{sl}]),"
        start_pos = 0
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

        self.buf_size = start_pos + 1
        return ("\n" + " " * 8).join(driver_inputs)

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
