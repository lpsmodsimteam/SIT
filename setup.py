#!/usr/bin/env python

from setuptools import setup

setup(
    name="bboxgen",
    version="1.0",
    description="SST Interoperable Boilerplate Black Box Generator",
    author="Sabbir Ahmed",
    author_email="ahmed_sabbir@bah.com",
    url="https://github.com/sabbirahm3d/ssti",
    packages=["boilerplate"],
    data_files=[
        ("template/pyrtl",
            ["template/pyrtl/driver.py",
             "template/pyrtl/comp.cpp"]
         ),
        ("template/systemc",
            ["template/systemc/driver.cpp",
             "template/systemc/comp.cpp"]
         )
    ],
)
