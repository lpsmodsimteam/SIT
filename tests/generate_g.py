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

PORT_DEFS = {
    "<bool> clock": "clock",
    "<bool> reset": "input",
    "<sc_uint<4> > data_out": "output",
}

boilerplate_sock = BoilerPlate(
    module="galois_lfsr",
    lib="proto",
    ipc="sock",
    drvr_templ_path=DRVR_TEMPL_PATH,
    bbox_templ_path=BBOX_TEMPL_PATH,
    desc="Simple 4-bit Galois Linear Feedback Shift Register",
    link_desc={
        "link_desc0": "Galois LFSR data_in",
        "link_desc1": "Galois LFSR data_out",
    }
)
boilerplate_sock.set_ports(PORT_DEFS)

boilerplate_zmq = BoilerPlate(
    module="galois_lfsr",
    lib="proto",
    ipc="zmq",
    drvr_templ_path=DRVR_TEMPL_PATH,
    bbox_templ_path=BBOX_TEMPL_PATH,
    desc="Simple 4-bit Galois Linear Feedback Shift Register",
    link_desc={
        "link_desc0": "Galois LFSR data_in",
        "link_desc1": "Galois LFSR data_out",
    }
)
boilerplate_zmq.set_ports(PORT_DEFS)


class TestBoilerPlate(unittest.TestCase):

    unittest.TestCase.maxDiff = None

    @staticmethod
    def read_file(path):

        with open(os.path.join(BBOX_DIR_PATH, path)) as file:
            return file.read()

    def test_driver_sock(self):

        self.assertTrue(
            difflib.SequenceMatcher(
                None,
                boilerplate_sock.generate_sc_drvr(),
                self.read_file("galois_sock_driver.cpp")).ratio() > 0.95,
            "Files diff too much"
        )

    def test_driver_zmq(self):

        self.assertTrue(
            difflib.SequenceMatcher(
                None,
                boilerplate_zmq.generate_sc_drvr(),
                self.read_file("galois_zmq_driver.cpp")).ratio() > 0.95,
            "Files diff too much"
        )

    def test_blackbox_sock(self):

        self.assertTrue(
            difflib.SequenceMatcher(
                None,
                boilerplate_sock.generate_bbox(),
                self.read_file("galois_sock.cpp")).ratio() > 0.90,
            "Files diff too much"
        )

    def test_blackbox_zmq(self):

        self.assertTrue(
            difflib.SequenceMatcher(
                None,
                boilerplate_zmq.generate_bbox(),
                self.read_file("galois_zmq.cpp")).ratio() > 0.90,
            "Files diff too much"
        )


if __name__ == "__main__":

    unittest.main()
