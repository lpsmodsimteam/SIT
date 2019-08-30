#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Implementation of the BoilerPlate class

This class generates the boilerplate code required to build the black box
interface in SSTI.
"""

import math
import os

from boilerplate import BoilerPlate


class PyRTL(BoilerPlate):

    def __init__(self, module, lib, ipc, drvr_templ_path, sst_model_templ_path,
                 desc="", link_desc=None, module_dir="", lib_dir=""):
        """Constructor for PyRTL BoilerPlate.

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
                         sst_model_templ_path, desc, link_desc,
                         module_dir, lib_dir)

        self.start_pos = 0

        if self.ipc in ("sock", "sock", "socket", "sockets"):
            self.sender = self.receiver = "m_signal_io"

            self.sst_model_decl = """SocketSignal m_signal_io;"""
            self.sst_model_init = "m_signal_io(0, socket(AF_UNIX, SOCK_STREAM, 0)),"
            self.sst_model_bind = "m_signal_io.set_addr(m_ipc_port)"
            self.sst_model_dest = ""

        elif self.ipc == "zmq":
            self.sst_model_decl = """zmq::context_t m_context;
    zmq::socket_t m_socket;
    ZMQReceiver m_signal_i;
    ZMQTransmitter m_signal_o;"""
            self.sst_model_init = """m_context(1), m_socket(m_context, ZMQ_REP),
      m_signal_i(0, m_socket), m_signal_o({n}_NPORTS, m_socket),"""
            self.sst_model_bind = "m_socket.bind(m_ipc_port.c_str())"
            self.sst_model_dest = "m_socket.close();"
            self.sender = "m_signal_o"
            self.receiver = "m_signal_i"

        self.sc_driver_path = self.sc_driver_path + "_driver.py"
        self.sst_model_path = self.sst_model_path + "_comp.cpp"

    def __parse_signal_type(self, signal):
        """Parses the type and computes its size from the signal

        Arguments:
            signal {str} -- signal definition

        Returns:
            {tuple(str,int)} -- C++ datatype and its size
        """
        # NoneTypes are explicitly assigned to SST component clock signals
        if signal == "1":
            return 1

        def __get_ints(sig):
            return int("".join(s for s in sig if s.isdigit()))

        return math.floor(math.log2(__get_ints(signal)))

    def get_inputs(self, driver=True):
        """Generates input bindings for both the components in the black box

        Arguments:
            driver {bool} -- option to generate code for the black box-driver
                (default: {True})

        Returns:
            {str} -- snippet of code representing input bindings
        """
        print(self.inputs)
        inputs = []
        fmt = "\"{sig}\" : int(signal[{sp}:{sl}]),"
        for driver_input in self.inputs:
            sig_len = self.__parse_signal_type(driver_input[0])
            inputs.append(
                fmt.format(
                    sp=self.start_pos,
                    sl=str(sig_len + self.start_pos),
                    sig=driver_input[-1],
                )
            )
            self.start_pos += sig_len

        return ("\n" + " " * 8).join(inputs)

    def get_outputs(self, driver=True):
        """Generates output bindings for both the components in the black box

        Arguments:
            driver {bool} -- option to generate code for the black box-driver
                (default: {True})

        Returns:
            {str} -- snippet of code representing output bindings
        """
        return self.sig_fmt(
            "str({module}.sim.inspect({module}.{sig})).encode()",
            lambda x: {
                "module": self.module,
                "sig": x[-1]
            }, self.outputs,
            " +\n" + " " * 8
        )

    def generate_driver(self):
        """Generates the black box-driver code based on methods used to format
        the template file

        Returns:
            {str} -- boilerplate code representing the black box-driver file
        """
        if os.path.isfile(self.drvr_templ_path):
            with open(self.drvr_templ_path) as template:
                return template.read().format(
                    module_dir=self.module_dir,
                    module=self.module,
                    inputs=self.get_inputs(),
                    sig_len=self.start_pos + 1,
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
                    lib_dir=self.lib_dir,
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
                )

        raise FileNotFoundError("Model boilerplate file not found")

    def generate_bbox(self):
        """Provides a high-level interface to the user to generate both the
        components of the black box and dump them to their corresponding files
        """
        if not len(self.ports):
            raise IndexError("Ports were not set properly")

        if not os.path.exists(self.bbox_dir):
            os.makedirs(self.bbox_dir)

        with open(self.sc_driver_path, "w") as driver_file:
            driver_file.write(self.generate_driver())

        with open(self.sst_model_path, "w") as sst_model_file:
            sst_model_file.write(self.generate_sst_model())


if __name__ == '__main__':

    import sys
    BASE_DIR = os.path.dirname(os.path.dirname(
        os.path.dirname(os.path.abspath(__file__))))
    SCRIPT_PATH = os.path.join(BASE_DIR, "ssti", "boilerplate")
    DRVR_TEMPL_PATH = os.path.join(
        SCRIPT_PATH, "template", "pyrtl", "driver.py")
    MODEL_TEMPL_PATH = os.path.join(
        SCRIPT_PATH, "template", "pyrtl", "model.cpp")

    ARGS = dict(
        module_dir="../../common/",
        lib_dir="../../../../ssti/",
        module="ram",
        lib="tests",
        drvr_templ_path=DRVR_TEMPL_PATH,
        sst_model_templ_path=MODEL_TEMPL_PATH,
        desc="Demonstration of a PyRTL hardware simulation in SST",
        link_desc={
            "link_desc0": "RAM data_in",
            "link_desc1": "RAM data_out",
        }
    )

    boilerplate_obj = PyRTL(**ARGS, ipc=sys.argv[-1])
    boilerplate_obj.set_ports((
        ("8", "address", "input"),
        ("1", "cs", "input"),
        ("1", "we", "input"),
        ("1", "oe", "input"),
        ("8", "data_in", "input"),
        ("8", "data_out", "output"),
    ))
    boilerplate_obj.generate_bbox()
