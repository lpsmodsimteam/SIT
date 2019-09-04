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
    package_dir={
        "boilerplate": "boilerplate"
    },
    data_files=[
        ("boilerplate/template/pyrtl",
            ["boilerplate/template/pyrtl/driver.py",
             "boilerplate/template/pyrtl/comp.cpp"]
         ),
        ("boilerplate/template/systemc",
            ["boilerplate/template/systemc/driver.cpp",
             "boilerplate/template/systemc/comp.cpp"]
         )
    ],
)
