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

    def __parse_signal_type(self, signal):
        """Parse the type and computes its size from the signal

        Parameters:
        -----------
        signal : str
            signal definition

        Returns:
        --------
        tuple2(str,int)
            C++ datatype and its size
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
                return math.floor(math.log2(__get_ints()))

            elif any(x in signal for x in ("bit", "logic")):
                return __get_ints()

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
            "_data_out << {sig}",
            lambda x: {
                "sig": x[-1],
            },
            self.ports["output"],
            ";\n" + " " * 8
        )

    def _get_driver_inputs(self):
        """Wrap base _generate_driver_inputs() method with overridden parameters

        Returns:
        --------
        func
            _generate_driver_inputs(*args)
        """
        return self._generate_driver_inputs(
            fmt="{sig} = std::stoi(_data_in.substr({sp}, {sl}));",
            start_pos=1,
            signal_type_parser=self.__parse_signal_type,
            clock_fmt="{sig} = std::stoi(_data_in.substr({sp})) % 2;",
        )

    def __get_driver_port_defs(self):
        """Generate port definitions for the black box-driver

        Returns:
        --------
        str
           string format of driver port definitions
        """
        return "sc_signal" + ";\n    sc_signal".join(" ".join(i) for i in self._get_all_ports())

    def __get_driver_bindings(self):
        """Generate port bindings for the black box-driver

        Returns:
        --------
        str
            snippet of code representing port bindings
        """
        return self._sig_fmt(
            "DUT.{sig}({sig})",
            lambda x: {"sig": x[-1]}, self._get_all_ports()
        )

    def _get_driver_defs(self):
        """Map definitions for the SystemC driver format string

        Returns:
        --------
        dict(str:str)
            format mapping of template SystemC driver string
        """
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
