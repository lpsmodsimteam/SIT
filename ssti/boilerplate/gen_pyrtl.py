#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Implementation of the BoilerPlate class

This class generates the boilerplate code required to build the black box
interface in SSTI.
"""

import math
import os
from string import punctuation

from boilerplate import BoilerPlate


class PyRTL(BoilerPlate):

    def __init__(self, module, lib, ipc, drvr_templ_path, sst_model_templ_path,
                 desc="", link_desc=None):
        """Constructor for BoilerPlate.

        Arguments:
            module {str} -- module name
            lib {str} -- SST library name
            ipc {str} -- type of IPC. Supported options are ("sock", "zmq")
            drvr_templ_path {str} -- path to the black box-driver boilerplate
            sst_model_templ_path {str} -- path to the black box-model boilerplate
            desc {str} -- description of the SST model (default: {""})
            link_desc {dict(str:str)} -- description of the SST links
                The argument defaults to:
                `{"link_desc0": "", "link_desc1": ""}`
                where they're assigned as the receiving and transmitting SST
                links respectively.
        """
        super().__init__(module, lib, ipc, drvr_templ_path,
                         sst_model_templ_path, desc, link_desc)

        self.driver_path = self.bbox_dir + "/" + self.module + "_driver.py"
        self.sst_model_path = self.bbox_dir + "/" + self.module + "_comp.cpp"

    @staticmethod
    def __parse_signal_type(signal, raw=True):
        """Parses the type and computes its size from the signal

        Arguments:
            signal {str} -- signal definition

        Returns:
            {tuple(str,int)} -- C++ datatype and its size
        """
        # NoneTypes are explicitly assigned to SST component clock signals
        if not signal:
            return signal, 0

        elif "sc" in signal:

            def __get_ints(sig):
                return int("".join(s for s in sig if s.isdigit())
                           if "//" not in sig else sig.split("//")[-1])

            if "int" in signal:
                return "<int>", math.floor(math.log2(__get_ints(signal)))

            if "bv" in signal or "lv" in signal:
                return signal.split("//")[0] if raw else "<int>", __get_ints(signal)

            if "bit" in signal or "logic" in signal:
                return "<bool>", __get_ints(signal)

        else:
            return signal, 1

    def get_driver_port_defs(self):
        """Generates port definitions for the black box-driver"""
        return "sc_signal" + ";\n    sc_signal".join(
            " ".join(i) for i in self.ports)

    def get_driver_bindings(self):
        """Generates port bindings for the black box-driver

        Returns:
            {str} -- snippet of code representing port bindings
        """
        return self.__format(
            "DUT.{sig}({sig})",
            lambda x: {"sig": x[-1]}, self.ports
        )

    def get_clock(self, driver=True):
        """Generates clock binding for both the components in the black box

        Arguments:
            driver {bool} -- option to generate code for the black box-driver
                (default: {True})

        Returns:
            {str} -- snippet of code representing clock binding
        """
        return self.__format(
            "{sig} = {recv}.get_clock_pulse({abbr}_ports.{sig})",
            lambda x: {
                "abbr": self.abbr,
                "recv": self.receiver,
                "sig": x[-1]
            }, self.clocks
        ) if driver else [[None, i[-1]] for i in self.clocks]

    def get_inputs(self, driver=True):
        """Generates input bindings for both the components in the black box

        Arguments:
            driver {bool} -- option to generate code for the black box-driver
                (default: {True})

        Returns:
            {str} -- snippet of code representing input bindings
        """
        return self.__format(
            "{sig} = {recv}.get{type}({abbr}_ports.{sig})",
            lambda x: {
                "abbr": self.abbr,
                "recv": self.receiver,
                "sig": x[-1],
                "type": self.__parse_signal_type(x[0])[0],
            }, self.inputs, ";\n" + " " * 8
        ) if driver else self.__format(
            "std::to_string({recv}.get{type}({abbr}_ports.{sig}))",
            lambda x: {
                "abbr": self.abbr,
                "recv": self.receiver,
                "sig": x[-1],
                "type": self.__parse_signal_type(x[0], raw=False)[0],
            }, self.outputs, " +\n" + " " * 16
        )

    def get_outputs(self, driver=True):
        """Generates output bindings for both the components in the black box

        Arguments:
            driver {bool} -- option to generate code for the black box-driver
                (default: {True})

        Returns:
            {str} -- snippet of code representing output bindings
        """
        if driver:

            return self.__format(
                "{send}.set({abbr}_ports.{sig}, {sig})",
                lambda x: {
                    "abbr": self.abbr,
                    "send": self.sender,
                    "sig": x[-1],
                }, self.outputs, ";\n" + " " * 8
            )

        # else, the SST component requires evaluation of signal type lengths
        # positions 0 and 1 are reserved for stopping sending and receiving of
        # signals
        start_pos = 2
        sst_model_inputs = self.inputs + self.get_clock(driver=False)
        sst_model_output = []
        fmt = "{send}.set({abbr}_ports.{sig}, std::stoi(_data_in.substr({sp}{sl})))"
        for model_input in sst_model_inputs:
            sig_len = self.__parse_signal_type(model_input[0])[-1]
            sst_model_output.append(
                fmt.format(
                    abbr=self.abbr,
                    sl=(", " + str(sig_len)) * bool(sig_len),
                    sp=start_pos,
                    send=self.sender,
                    sig=model_input[-1],
                )
            )
            start_pos += sig_len

        return (";\n" + " " * 8).join(sst_model_output)

    def generate_driver(self):
        """Generates the black box-driver code based on methods used to format
        the template file

        Returns:
            {str} -- boilerplate code representing the black box-driver file
        """
        if os.path.isfile(self.drvr_templ_path):
            with open(self.drvr_templ_path) as template:
                return template.read().format(
                    abbr=self.abbr,
                    module=self.module,
                    port_defs=self.get_driver_port_defs(),
                    bindings=self.get_driver_bindings(),
                    var_decl=self.driver_decl,
                    dest=self.drvr_dest,
                    clock=self.get_clock(),
                    sender=self.sender,
                    receiver=self.receiver,
                    inputs=self.get_inputs(),
                    outputs=self.get_outputs()
                )

        raise FileNotFoundError("Driver boilerplate file not found")

    def generate_sst_model(self):
        """Generates the black box-model code based on methods used to format
        the template file

        Returns:
            {str} -- boilerplate code representing the black box-model file
        """
        if os.path.isfile(self.sst_model_templ_path):
            with open(self.sst_model_templ_path) as template:
                return template.read().format(
                    abbr=self.abbr,
                    module=self.module,
                    lib=self.lib,
                    comp=self.module,
                    desc=self.desc,
                    **self.link_desc,
                    var_decl=self.sst_model_decl,
                    var_init=self.sst_model_init,
                    var_bind=self.sst_model_bind,
                    var_dest=self.sst_model_dest,
                    sender=self.sender,
                    receiver=self.receiver,
                    inputs=self.get_inputs(False),
                    outputs=self.get_outputs(False),
                )

        raise FileNotFoundError("Model boilerplate file not found")

    def generate_ports_enum(self):
        """Generates the number of ports and the port enumeration for the black
        box components

        Returns:
            {str} -- boilerplate code representing the black box port details
        """
        return self.enums.format(
            abbr=self.abbr,
            abbrU=self.abbr.upper(),
            nports=len(self.ports) + 1,
            ports=self.__format(
                "unsigned short int {port} = {n}",
                lambda x: {
                    "port": x[-1][-1],
                    "n": x[0] + 1
                }, enumerate(self.ports)
            )
        )

    def generate_bbox(self):
        """Provides a high-level interface to the user to generate both the
        components of the black box and dump them to their corresponding files
        """
        if not len(self.ports):
            raise IndexError("Ports were not set properly")

        if not os.path.exists(self.bbox_dir):
            os.makedirs(self.bbox_dir)

        with open(self.driver_path, "w") as sc_driver_file:
            sc_driver_file.write(self.generate_driver())

        with open(self.sst_model_path, "w") as sst_model_file:
            sst_model_file.write(self.generate_sst_model())
