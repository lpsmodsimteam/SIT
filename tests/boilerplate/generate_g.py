#!/usr/bin/env python
# -*- coding: utf-8 -*-

import difflib
import os
import sys
import unittest

BASE_DIR = os.path.dirname(os.getcwd())
SCRIPT_PATH = os.path.join(BASE_DIR, "ssti", "boilerplate")
SOCKS_BBOX_DIR = os.path.join(
    BASE_DIR, "tests", "systemc", "socks", "blackboxes")
ZMQ_BBOX_DIR = os.path.join(
    BASE_DIR, "tests", "systemc", "zmq", "blackboxes")
DRVR_TEMPL_PATH = os.path.join(SCRIPT_PATH, "template", "systemc", "driver.cpp")
MODEL_TEMPL_PATH = os.path.join(SCRIPT_PATH, "template", "systemc", "model.cpp")
sys.path.append(SCRIPT_PATH)
from gen_systemc import SystemC


class TestBoilerplate(object):

    def __init__(self, boilerplate_obj, base_path, driver_path, model_path):

        self.driver_diffs = """"""

        self.model_diffs = """"""
        self.boilerplate_obj = boilerplate_obj
        self.boilerplate_obj.set_ports((
            ("<sc_bv<ADDR_WIDTH>>//8", "address", "input"),
            ("<bool>", "cs", "input"),
            ("<bool>", "we", "input"),
            ("<bool>", "oe", "input"),
            ("<sc_bv<DATA_WIDTH>>//8", "data_in", "input"),
            ("<sc_bv<DATA_WIDTH>>//8", "data_out", "output"),
        ))
        self.boilerplate_obj.generate_bbox()
        self.driver_path = os.path.join(base_path, driver_path)
        self.model_path = os.path.join(base_path, model_path)

    @staticmethod
    def read_file(path):

        with open(path) as file:
            return file.read()

    @staticmethod
    def remove_diff_trails(file1, file2):

        diff = difflib.context_diff(
            file1.splitlines(keepends=True),
            file2.splitlines(keepends=True), n=0)
        filtered_diff = []
        for line in diff:
            if "::" in line:
                print(line)
            if not ("***" in line or "---" in line):
                filtered_diff.append(line)

        return "".join(filter(lambda x: len(set(x)) > 3, filtered_diff))

    def test_driver(self):

        with open(self.boilerplate_obj.sc_driver_path) as driver_file:
            self.assertEqual(
                self.remove_diff_trails(
                    driver_file.read(), self.read_file(self.driver_path)
                ),
                self.driver_diffs,
                "Generated boilerplate driver code is not accurate"
            )

    def test_model(self):

        with open(self.boilerplate_obj.sst_model_path) as model_file:
            self.assertEqual(
                self.remove_diff_trails(
                    model_file.read(), self.read_file(self.model_path)
                ),
                self.model_diffs,
                "Generated boilerplate model code is not accurate"
            )


ARGS = dict(
    module_dir="../../common/",
    ports_dir="../../common/blackboxes/",
    lib_dir="../../../../ssti/",
    module="ram",
    lib="systemc",
    drvr_templ_path=DRVR_TEMPL_PATH,
    sst_model_templ_path=MODEL_TEMPL_PATH,
    desc="Demonstration of a SystemC hardware simulation in SST",
    link_desc={
        "link_desc0": "RAM data_in",
        "link_desc1": "RAM data_out",
    }
)


class SocketSignals(unittest.TestCase, TestBoilerplate):

    def setUp(self):

        TestBoilerplate.__init__(self, SystemC(**ARGS, ipc="sock"),
                                 SOCKS_BBOX_DIR, "ram_driver.cpp", "ram_comp.cpp")


class ZMQSignals(unittest.TestCase, TestBoilerplate):

    def setUp(self):

        TestBoilerplate.__init__(self, SystemC(**ARGS, ipc="zmq"),
                                 ZMQ_BBOX_DIR, "ram_driver.cpp", "ram_comp.cpp")


if __name__ == "__main__":

    unittest.TestCase.maxDiff = None
    unittest.main()
