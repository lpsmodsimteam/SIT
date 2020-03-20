#!/usr/bin/env python

import os
from setuptools import setup

from boilerplate import boilerplate

TEMPL_DIR = "boilerplate/template/"
HDL = (TEMPL_DIR + hdl for hdl in ("chisel", "pyrtl", "systemc", "verilog"))

setup(
    name="boilerplate",
    version=os.environ["SIT_VERSION"],
    description="SST Interoperability Toolkit Boilerplate Black Box Generator",
    long_description=boilerplate.__doc__,
    author="Sabbir Ahmed",
    author_email="ahmed_sabbir@bah.com",
    url="https://github.com/sabbirahm3d/sit",
    packages=["boilerplate"],
    package_dir={
        "boilerplate": "boilerplate"
    },
    data_files=[
        (hdl, [hdl + "/" + f for f in os.listdir(hdl)]) for hdl in HDL
    ],
)
