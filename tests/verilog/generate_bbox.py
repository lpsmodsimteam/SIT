import sys

from boilerplate import Verilog

if __name__ == "__main__":

    ipc = sys.argv[-1]

    boilerplate_obj = Verilog(
        ipc=ipc,
        module="ram",
        lib="verilog" + ipc,
        desc="Demonstration of a Verilog hardware simulation in SST",
        module_dir="../tests/verilog/common/",
    )
    boilerplate_obj.set_ports(
        (
            ("input", "address", "8"),
            ("input", "cs", "1"),
            ("input", "we", "1"),
            ("input", "oe", "1"),
            ("input", "data_in", "8"),
            ("output", "data_out", "8b"),
        )
    )
    boilerplate_obj.generate_bbox()
