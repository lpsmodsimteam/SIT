#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys

import add


add.sim.step({
    "op1": 4,
    "op2": 2,
})
print(add.sim.inspect(add.out))
