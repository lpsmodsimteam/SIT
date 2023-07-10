from sit import SIT

if __name__ == "__main__":

    config = {
        "hdl": "systemc",
        "config": {
            "module_name": "ram",
            "lib": "systemcsock",
            "desc": "Demonstration of a SystemC hardware simulation in SST",
            "module_dir": "../tests/systemc/",
        },
        "extra": {
            "width_macros": {
                "ADDR_WIDTH": 8,
                "DATA_WIDTH": 8,
            }
        },
        "ports": {
            "input": [
                {"name": "address", "type": "sc_bv<ADDR_WIDTH>", "len": 8},
                {"name": "cs", "type": "bool", "len": 1},
                {"name": "we", "type": "bool", "len": 1},
                {"name": "oe", "type": "bool", "len": 1},
                {"name": "data_in", "type": "sc_bv<DATA_WIDTH>", "len": 8},
            ],
            "output": [
                {"name": "data_out", "type": "sc_bv<DATA_WIDTH>", "len": 8}
            ],
        },
    }

    sit_obj = SIT(config)
    sit_obj.generate_black_boxes()
