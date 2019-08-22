#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys

BASE_DIR = os.path.dirname(os.path.dirname(os.getcwd()))
sys.path.append(BASE_DIR)
from params import test_setup

test_setup("sock", "unit")
