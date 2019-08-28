#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

if __name__ == "__main__":

    with open(sys.argv[-1]) as mem_dump_file:
        data = filter(None, mem_dump_file.read().split("\n"))
        assert ''.join([chr(int(x, 2)) for x in data]) == "HELLO WORLD"
        print("\x1b[1;32mOutputs match\x1b[0m")
