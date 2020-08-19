#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from boilerplate import Chisel

if __name__ == "__main__":

    ipc = sys.argv[-1]

    boilerplate_obj = Chisel(
        ipc=ipc,
        module="ram",
        lib="chisel" + ipc,
        desc="Demonstration of a Chisel hardware simulation in SST",
        module_dir="../../tests/chisel/common/",
    )
    boilerplate_obj.set_ports((
        ("input", "address", "8"),
        ("input", "cs", "1"),
        ("input", "we", "1"),
        ("input", "oe", "1"),
        ("input", "data_in", "8"),
        ("output", "data_out", "8", 8),
    ))
    boilerplate_obj.generate_bbox()
