#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os

PACKAGE_PARENT = '.'
SCRIPT_DIR = os.path.dirname(os.path.realpath(os.path.join(os.getcwd(), os.path.expanduser(__file__))))
sys.path.append(os.path.normpath(os.path.join(SCRIPT_DIR, PACKAGE_PARENT)))

from util import galois_lfsr_comp, fib_lfsr_comp, galois_file, fib_file

# overide default parameters
galois_lfsr_comp.addParams({
    "ipc_port": "ipc:///tmp/" + galois_file,
})

# overide default parameters
fib_lfsr_comp.addParams({
    "ipc_port": "ipc:///tmp/" + fib_file,
})
