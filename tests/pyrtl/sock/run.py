#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys

BASE_DIR = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "testcases")
sys.path.append(BASE_DIR)
from params import test_setup

test_setup("sock", "unit")
