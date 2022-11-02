#!/usr/bin/env python

import os
from pathlib import Path
import re
from setuptools import setup

from sit import sit

os.chdir(Path(__file__).parents[0])
TEMPL_DIR = Path("sit") / "template"
version_re = re.compile(r"\d.\d.\d")
setup(
    name="sit",
    version=version_re.findall(Path("version").read_text())[0],
    description="SST Interoperability Toolkit Boilerplate Black Box Generator",
    long_description=sit.__doc__,
    author="Sabbir Ahmed",
    author_email="ahmed_sabbir@bah.com",
    url="https://github.com/lpsmodsim/sit",
    packages=["sit"],
    package_dir={"sit": "sit"},
    package_data={
        "sit": [str(Path(*i.parts[1:])) for i in TEMPL_DIR.glob("*/*")]
        + ["py.typed"]
    },
)
