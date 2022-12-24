from sit import SITPyRTL, SITSystemC, SITVerilog

if __name__ == "__main__":

    ARGS = {"lib": "monte_carlo", "module_dir": "../"}

    sc_div = SITSystemC(
        **ARGS,
        module_name="sc_div",
    )
    sc_div.set_ports(
        (
            ("input", "op1", "<sc_uint<32>>"),
            ("input", "op2", "<sc_uint<32>>"),
            ("output", "data_out", "<float>", 12),
        )
    )
    sc_div.fixed_width_float_output(9)
    sc_div.disable_runtime_warnings(["SC_ID_NO_SC_START_ACTIVITY_"])
    sc_div.generate_black_box()

    #     mutation = PyRTL(
    #         **ARGS,
    #         module_name="mutation",
    #     )
    #     mutation.set_ports(
    #         (
    #             ("input", "chance", "3"),
    #             ("input", "gene", "3"),
    #             ("output", "out", "2"),
    #         )
    #     )
    #     mutation.generate_bbox()

    # elif sys.argv[-1] == "verilog":

    #     flash_mem = Verilog(
    #         lib="epidemic",
    #         ipc="sock",
    #         module_name="flash_mem",
    #         module_dir="../verilog/",
    #     )
    #     flash_mem.set_ports(
    #         (
    #             ("input", "address", "10"),
    #             ("input", "cs", "1"),
    #             ("input", "oe", "1"),
    #             ("input", "we", "1"),
    #             ("input", "data_in", "25"),
    #             ("output", "data_out", "25b"),
    #         )
    #     )
    #     flash_mem.generate_bbox()
