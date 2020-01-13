#!/usr/bin/env python

import os
from setuptools import setup

from boilerplate import boilerplate

HDL = ("chisel", "pyrtl", "systemc")

setup(
    name="boilerplate",
    version=os.environ["SIT_VERSION"],
    description="SST Interoperable Boilerplate Black Box Generator",
    long_description=boilerplate.__doc__,
    author="Sabbir Ahmed",
    author_email="ahmed_sabbir@bah.com",
    url="https://github.com/sabbirahm3d/sit",
    packages=["boilerplate"],
    package_dir={
        "boilerplate": "boilerplate"
    },
    data_files=[
        ("boilerplate/template/" + hdl,
         ["boilerplate/template/" + hdl + "/driver",
          "boilerplate/template/" + hdl + "/comp"]
         ) for hdl in HDL]
)
