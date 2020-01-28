#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys

from add import *


sim.step({
    "op1": 4,
    "op2": 2,
})
print(sim.inspect(out))
