#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Implementation of the SystemC class

This class inherits from the BoilerPlate base class and implements its own methods of parsing,
modifying and generating boilerplate code for its specific paradigms.
"""

import math

from .boilerplate import BoilerPlate


class SystemC(BoilerPlate):

    def __init__(self, ipc, module, lib, width_macros=None, module_dir="", lib_dir="", desc="",
                 driver_template_path="", component_template_path=""):
        """Constructor for SystemC BoilerPlate

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
            width_macros=width_macros,
            module_dir=module_dir,
            lib_dir=lib_dir,
            desc=desc,
            driver_template_path=driver_template_path,
            component_template_path=component_template_path
        )

        if self.ipc == "sock":

            # driver attributes
            self.sig_type = "SocketSignal"

        elif self.ipc == "zmq":

            # driver attributes
            self.sig_type = "ZMQSignal"

        self.driver_path += "_driver.cpp"
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
        def __get_ints():
            """Extract integers from signal string
            This is a closure method to keep scope local

            Returns:
            --------
            int
                integers found in signal string
            """
            try:
                return int("".join(s for s in signal if s.isdigit()))
            except ValueError:
                return int(self._get_signal_width(signal))

        # NoneTypes are explicitly assigned to SystemC model clock signals
        if not signal:
            return 0

        elif "sc" in signal:

            if any(x in signal for x in ("bv", "lv", "int")):
                # floor(log(-1 + 2^x)/log(10)) + 1
                return math.floor(math.log(math.pow(2, __get_ints()) - 1) / math.log(10)) + 1

            elif any(x in signal for x in ("bit", "logic")):
                return __get_ints()

        elif signal == "<float>":
            return 10

        else:
            return 1

    def _get_driver_outputs(self):
        """Generate output bindings for both the components in the black box

        Returns:
        --------
        str
            snippet of code representing output bindings
        """
        return self._sig_fmt(
            "_data_out << {precision}{sig}",
            lambda x: {
                "sig": x["name"],
                "precision": f"std::fixed << std::setprecision({self.precision}) << "
                if self.precision else "",
            },
            self.ports["output"],
            ";\n" + " " * 8
        )

    def _get_driver_inputs(self):
        """Generate input bindings for the driver.

        Returns:
        --------
        str
            snippet of code representing input bindings
        """
        fmt = "{sig} = std::sto{type}(_data_in.substr({sp}, {sl}));"
        start_pos = 1
        clock_fmt = "{sig} = std::stoi(_data_in.substr({sp}, {sl})) % 2;"
        driver_inputs = []

        # input_port = (INPUT NAME, INPUT TYPE)
        for input_port in self.ports["input"]:
            driver_inputs.append(
                fmt.format(
                    type="f" if input_port["type"] == "<float>" else "i",
                    sp=start_pos,
                    sl=str(input_port["len"]),
                    sig=input_port["name"],
                )
            )
            start_pos += input_port["len"]

        if self.ports["clock"]:
            for clock_port in self.ports["clock"]:
                driver_inputs.append(
                    clock_fmt.format(
                        sp=start_pos,
                        sl=clock_port["len"],
                        sig=clock_port["name"]
                    )
                )
                start_pos += int(clock_port["len"])

        self.buf_size = start_pos
        return ("\n" + " " * 8).join(driver_inputs)

    def __get_driver_port_defs(self):
        """Generate port definitions for the black box-driver

        Returns:
        --------
        str
           string format of driver port definitions
        """
        return "sc_signal" + ";\n    sc_signal".join(i["type"] + " " + i["name"] for i in self._get_all_ports())

    def __get_driver_bindings(self):
        """Generate port bindings for the black box-driver

        Returns:
        --------
        str
            snippet of code representing port bindings
        """
        return self._sig_fmt(
            "DUT.{sig}({sig})",
            lambda x: {"sig": x["name"]}, self._get_all_ports()
        )

    def _get_driver_defs(self):
        """Map definitions for the SystemC driver format string

        Returns:
        --------
        dict(str:str)
            format mapping of template SystemC driver string
        """
        return {
            "extra_libs": self.extra_libs,
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

    def _fixed_width_float_output(self):

        self.extra_libs += "#include <iomanip>"
