#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys

BASE_DIR = os.path.dirname(os.getcwd())
SCRIPT_PATH = os.path.join(BASE_DIR, "demo")
sys.path.append(SCRIPT_PATH)
from params import unit_test

unit_test("zmq", "test")
