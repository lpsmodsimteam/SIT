#!/usr/bin/env python

import os
from pathlib import Path
import re
from setuptools import setup

from boilerplate import boilerplate

os.chdir(Path(__file__).parents[0])
TEMPL_DIR = Path("boilerplate") / "template"
version_re = re.compile(r"\d.\d.\d")
print([str(Path(*i.parts[1:])) for i in TEMPL_DIR.glob("*/*")])
setup(
    name="boilerplate",
    version=version_re.findall(Path("version").read_text())[0],
    description="SST Interoperability Toolkit Boilerplate Black Box Generator",
    long_description=boilerplate.__doc__,
    author="Sabbir Ahmed",
    author_email="ahmed_sabbir@bah.com",
    url="https://github.com/sabbirahm3d/sit",
    packages=["boilerplate"],
    package_dir={
        "boilerplate": "boilerplate"
    },
    package_data={
        "boilerplate": [str(Path(*i.parts[1:])) for i in TEMPL_DIR.glob("*/*")]
    },
)
