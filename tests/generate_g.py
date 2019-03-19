#!/usr/bin/env python
# -*- coding: utf-8 -*-

import difflib
import os
import sys
import unittest

BASE_DIR = os.getcwd()
SCRIPT_PATH = os.path.join(BASE_DIR, "src", "boilerplate")
BBOX_DIR_PATH = os.path.join(BASE_DIR, "proto", "blackboxes")
DRVR_TEMPL_PATH = os.path.join(SCRIPT_PATH, "template", "driver.tmp")
BBOX_TEMPL_PATH = os.path.join(SCRIPT_PATH, "template", "blackbox.tmp")
sys.path.append(SCRIPT_PATH)
from generate import BoilerPlate


class TestBoilerPlate(unittest.TestCase):

    def __init__(self, methodName="runTest"):

        super(TestBoilerPlate, self).__init__(methodName)

        ARGS = dict(
            module="galois_lfsr",
            lib="proto",
            drvr_templ_path=DRVR_TEMPL_PATH,
            bbox_templ_path=BBOX_TEMPL_PATH,
            desc="Simple 4-bit Galois Linear Feedback Shift Register",
            link_desc={
                "link_desc0": "Galois LFSR data_in",
                "link_desc1": "Galois LFSR data_out",
            }
        )
        PORT_DEFS = {
            "<bool> clock": "clock",
            "<bool> reset": "input",
            "<sc_uint<4> > data_out": "output",
        }

        self.boilerplate_sock = BoilerPlate(**ARGS, ipc="sock")
        self.boilerplate_sock.set_ports(PORT_DEFS)

        self.boilerplate_zmq = BoilerPlate(**ARGS, ipc="zmq")
        self.boilerplate_zmq.set_ports(PORT_DEFS)

        self.driver_diffs = """! #include "galois_lfsr.hpp"
! #include "../modules/galois_lfsr.hpp"
! #include "sstscit.hpp"
! #include "../../src/sstscit.hpp"
"""

        self.blackbox_diffs = """! #include "sstscit.hpp"
! #include "../../src/sstscit.hpp"
"""

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

    def test_driver_sock(self):

        diffs = self.remove_diff_trails(
            self.boilerplate_sock.generate_sc_drvr(),
            self.read_file("galois_sock_driver.cpp"))

        self.assertEqual(
            diffs, self.driver_diffs,
            "Generated boilerplate code is not accurate"
        )

    def test_driver_zmq(self):

        diffs = self.remove_diff_trails(
            self.boilerplate_zmq.generate_sc_drvr(),
            self.read_file("galois_zmq_driver.cpp"))

        self.assertEqual(
            diffs, self.driver_diffs,
            "Generated boilerplate code is not accurate"
        )

    def test_blackbox_sock(self):

        diffs = self.remove_diff_trails(
            self.boilerplate_sock.generate_bbox(),
            self.read_file("galois_sock.cpp"))
        self.assertEqual(
            diffs, self.blackbox_diffs,
            "Generated boilerplate code is not accurate"
        )

    def test_blackbox_zmq(self):

        diffs = self.remove_diff_trails(
            self.boilerplate_zmq.generate_bbox(),
            self.read_file("galois_zmq.cpp"))
        self.assertEqual(
            diffs, self.blackbox_diffs,
            "Generated boilerplate code is not accurate"
        )


if __name__ == "__main__":

    unittest.main()
