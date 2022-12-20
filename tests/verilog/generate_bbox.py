from sit import SITVerilog

if __name__ == "__main__":

    sit_obj = SITVerilog(
        module_name="ram",
        lib="verilogsock",
        desc="Demonstration of a Verilog hardware simulation in SST",
        module_dir="../tests/verilog/common/",
    )
    sit_obj.set_ports(
        (
            ("input", "address", "8"),
            ("input", "cs", "1"),
            ("input", "we", "1"),
            ("input", "oe", "1"),
            ("input", "data_in", "8"),
            ("output", "data_out", "8b"),
        )
    )
    sit_obj.generate_black_boxes()
