from sit import SITSystemC

if __name__ == "__main__":

    sit_obj = SITSystemC(
        module_name="ram",
        lib="systemcsock",
        desc="Demonstration of a SystemC hardware simulation in SST",
        module_dir="../tests/systemc/",
        width_macros={
            "ADDR_WIDTH": 8,
            "DATA_WIDTH": 8,
        },
    )
    sit_obj.set_ports(
        (
            ("input", "address", "<sc_bv<ADDR_WIDTH>>"),
            ("input", "cs", "<bool>"),
            ("input", "we", "<bool>"),
            ("input", "oe", "<bool>"),
            ("input", "data_in", "<sc_bv<DATA_WIDTH>>"),
            ("output", "data_out", "<sc_bv<DATA_WIDTH>>", 8),
        )
    )
    sit_obj.generate_black_boxes()
