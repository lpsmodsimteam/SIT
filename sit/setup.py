#!/usr/bin/env python

import os
from setuptools import setup

setup(
    name="boilerplate",
    version=os.environ["SIT_VERSION"],
    description="SST Interoperable Boilerplate Black Box Generator",
    author="Sabbir Ahmed",
    author_email="ahmed_sabbir@bah.com",
    url="https://github.com/sabbirahm3d/sit",
    packages=["boilerplate"],
    package_dir={
        "boilerplate": "boilerplate"
    },
    data_files=[
        ("boilerplate/template/pyrtl",
         ["boilerplate/template/pyrtl/driver",
          "boilerplate/template/pyrtl/comp"]
         ),
        ("boilerplate/template/systemc",
         ["boilerplate/template/systemc/driver",
          "boilerplate/template/systemc/comp"]
         )
    ],
)
