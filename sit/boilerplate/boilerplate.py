#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os


class BoilerPlate(object):

    def __init__(self, ipc, module, lib, macros=None, module_dir="", lib_dir="", desc="",
                 driver_template_path="", component_template_path=""):
        """Constructor for BoilerPlate

        Initialize all member port variables and component variables

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

        Raises:
        -------
        ValueError
            unsupported IPC is provided
        """
        if ipc in ("sock", "zmq"):
            self.ipc = ipc
        else:
            raise ValueError("Incorrect or unsupported IPC protocol provided")

        self.module = module
        self.lib = lib
        self.module_dir = module_dir
        self.lib_dir = lib_dir
        self.desc = desc

        template_path = os.path.join(
            os.path.dirname(__file__), "template", self.__class__.__name__.lower())
        if not driver_template_path:
            self.driver_template_path = os.path.join(template_path, "driver")
        if not component_template_path:
            self.component_template_path = os.path.join(template_path, "comp")

        self.macros = macros if macros else {}
        self.ports = {
            "clock": [],
            "input": [],
            "output": [],
            "inout": []
        }
        self.buf_size = 0

        if self.ipc == "sock":

            # component attributes
            self.sig_type = """SocketSignal"""

        elif self.ipc == "zmq":

            # component attributes
            self.sig_type = """ZMQSignal"""

        # shared attributes
        self.sender = self.receiver = "m_signal_io"
        self.bbox_dir = "blackboxes"
        self.driver_path = self.comp_path = os.path.join(self.bbox_dir, self.module)

    @staticmethod
    def _sig_fmt(fmt, split_func, array, delim=";\n    "):
        """Formats lists of signals based on fixed arguments

        Parameters:
        -----------
        fmt : str
            string format
        split_func : lambda or dict(str: str)
            map to split on the signals
        array : list(str)
            list of signals
        delim : str (default: ";n    ")
            delimiter

        Returns:
        --------
        str
            string formatted signals
        """
        return delim.join(fmt.format(**split_func(i)) for i in array)

    def _get_signal_width(self, signal):
        for macro, value in self.macros.items():
            if macro in signal:
                return value
        raise AttributeError(f"Invalid macro in signal: {signal}")

    def _get_all_ports(self):

        return (i for sig in self.ports.values() for i in sig)

    def set_ports(self, ports):
        """Assigns ports to their corresponding member lists

        Parameters:
        -----------
        ports : tuple(tuple3(str))
            C++-style type-declared signals in the form (<DATA TYPE>, <PORT NAME>, <PORT TYPE>).
            The current types of signals supported are ("clock", "input", "output", "inout")

        Raises:
        -------
        IndexError
            invalid signal format provided
        TypeError
            invalid port type provided
        """
        if set(len(port) for port in ports) != {3}:
            raise IndexError(
                """Invalid signal format. Make sure signals are grouped in the format:
(<DATA TYPE>, <PORT NAME>, <PORT TYPE>)""")

        for port_data_type, port_name, port_type in ports:
            try:
                self.ports[port_type].append((port_data_type, port_name))
            except KeyError:
                raise TypeError(f"""{port_type} is an invalid port type. Valid port types are:
("clock", "input", "output", "inout")""")

    def _generate_driver_inputs(self, fmt, start_pos, signal_type_parser, splice=False,
                                clock_fmt=""):
        """Generates input bindings for the driver.

        Parameters:
        -----------
        fmt : str
            format string for inputs
        start_pos : int
            starting position of signals
        signal_type_parser : func or lambda
            signal parser
        splice : bool (default: False)
            if signals are being spliced from a buffer.
            If true, the starting position is added to the signal length.
        clock_fmt : str (default: "")
            special string format for clock signals

        Returns:
        --------
        str
            snippet of code representing input bindings
        """
        driver_inputs = []
        for input_type, input_name in self.ports["input"]:
            sig_len = signal_type_parser(input_type)
            driver_inputs.append(
                fmt.format(
                    sp=start_pos,
                    sl=str(sig_len + (splice * start_pos)),
                    sig=input_name,
                )
            )
            start_pos += sig_len

        if self.ports["clock"]:
            for clock_type, clock_name in self.ports["clock"]:
                driver_inputs.append(
                    clock_fmt.format(sp=start_pos, sig=clock_name)
                )
                start_pos += int(self._get_signal_width(clock_type))

        self.buf_size = start_pos + 1 if splice else start_pos
        return ("\n" + " " * 8).join(driver_inputs)

    def __get_comp_defs(self):
        """Map definitions for the component format string

        Returns:
        --------
        dict(str:str)
            format mapping of template component string
        """
        return {
            "lib_dir": self.lib_dir,
            "module": self.module,
            "lib": self.lib,
            "desc": self.desc,
            "ports": self._sig_fmt(
                """{{ "{link}", "{desc}", {{ "sst.Interfaces.StringEvent" }}}}""",
                lambda x: {
                    "link": self.module + x[0],
                    "desc": self.module + x[-1]
                },
                (("_din", " data in"), ("_dout", " data out")),
                ",\n" + " " * 8
            ),
            "sig_type": self.sig_type,
            "buf_size": self.buf_size,
            "sender": self.sender,
            "receiver": self.receiver,
        }

    def __generate_comp_str(self):
        """Generates the black box-model code based on methods used to format
        the template file

        Raises:
        -------
        FileNotFoundError
            boilerplate template file not found

        Returns:
        --------
        str
            boilerplate code representing the black box-model file
        """
        if os.path.isfile(self.component_template_path):
            with open(self.component_template_path) as template:
                return template.read().format(
                    **self.__get_comp_defs()
                )

        raise FileNotFoundError("Component boilerplate template file not found")

    def __generate_driver_str(self):
        """Generates the black box-driver code based on methods used to format
        the template file

        Raises:
        -------
        FileNotFoundError
            boilerplate template file not found

        Returns:
        --------
        str
            boilerplate code representing the black box-driver file
        """
        if os.path.isfile(self.driver_template_path):
            with open(self.driver_template_path) as template:
                return template.read().format(
                    inputs=self._get_driver_inputs(),
                    outputs=self._get_driver_outputs(),
                    **self._get_driver_defs()
                )

        raise FileNotFoundError("Driver boilerplate template file not found")

    def generate_bbox(self):
        """Provides a high-level interface to the user to generate both the
        components of the black box and dump them to their corresponding files

        Raises:
        -------
        ValueError
            no ports were provided
        """
        if not len(self.ports):
            raise ValueError("""Ports were not set properly.
Make sure to call set_ports() before generating files.""")

        if not os.path.exists(self.bbox_dir):
            os.makedirs(self.bbox_dir)

        with open(self.driver_path, "w") as driver_file:
            driver_file.write(self.__generate_driver_str())

        with open(self.comp_path, "w") as comp_file:
            comp_file.write(self.__generate_comp_str())
