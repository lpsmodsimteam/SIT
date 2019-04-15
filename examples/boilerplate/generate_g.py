#!/usr/bin/env python
# -*- coding: utf-8 -*-

import difflib
import os
import sys
import unittest

BASE_DIR = os.path.dirname(os.getcwd())
SCRIPT_PATH = os.path.join(BASE_DIR, "src", "boilerplate")
BBOX_DIR_PATH = os.path.join(BASE_DIR, "examples", "simple", "blackboxes")
DRVR_TEMPL_PATH = os.path.join(SCRIPT_PATH, "template", "driver.tmp")
MODEL_TEMPL_PATH = os.path.join(SCRIPT_PATH, "template", "model.tmp")
sys.path.append(SCRIPT_PATH)
from generate import BoilerPlate


class TestBoilerplate(object):

    def __init__(self, boilerplate_obj, driver_path, model_path):

        self.driver_diffs = """! #include "galois_lfsr.hpp"
! #include "../modules/galois_lfsr.hpp"
! #include "sstscit.hpp"
! #include "../../../src/sstscit.hpp"
"""

        self.model_diffs = """! #include "sstscit.hpp"
! #include "../../../src/sstscit.hpp"
"""
        self.boilerplate_obj = boilerplate_obj
        self.boilerplate_obj.set_ports({
            "<bool> clock": "clock",
            "<bool> reset": "input",
            "<sc_uint<4> > data_out": "output",
        })
        self.boilerplate_obj.generate_bbox()
        self.driver_path = driver_path
        self.model_path = model_path

    @staticmethod
    def read_file(path):

        with open(os.path.join(BBOX_DIR_PATH, path)) as file:
            return file.read()

    @staticmethod
    def remove_diff_trails(file1, file2):

        diff = difflib.context_diff(
            file1.splitlines(keepends=True),
            file2.splitlines(keepends=True), n=0)
        filtered_diff = []
        for line in diff:
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
    module="galois_lfsr",
    lib="proto",
    drvr_templ_path=DRVR_TEMPL_PATH,
    sst_model_templ_path=MODEL_TEMPL_PATH,
    desc="Simple 4-bit Galois Linear Feedback Shift Register",
    link_desc={
        "link_desc0": "Galois LFSR data_in",
        "link_desc1": "Galois LFSR data_out",
    }
)


class SocketSignals(unittest.TestCase, TestBoilerplate):

    def setUp(self):

        TestBoilerplate.__init__(self, BoilerPlate(**ARGS, ipc="sock"),
                                 "galois_sock_driver.cpp", "galois_sock.cpp")


class ZMQSignals(unittest.TestCase, TestBoilerplate):

    def setUp(self):

        TestBoilerplate.__init__(self, BoilerPlate(**ARGS, ipc="zmq"),
                                 "galois_zmq_driver.cpp", "galois_zmq.cpp")


if __name__ == "__main__":

    unittest.main()
