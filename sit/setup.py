#!/usr/bin/env python

import os
from setuptools import setup

from boilerplate import boilerplate

HDL = ("chisel", "pyrtl", "systemc")
TEMPL_DIR = "boilerplate/template/"

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
        (TEMPL_DIR + hdl,
         [TEMPL_DIR + hdl + "/driver",
          TEMPL_DIR + hdl + "/comp"]
         ) for hdl in HDL
    ],
)
