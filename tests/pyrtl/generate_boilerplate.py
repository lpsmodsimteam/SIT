from sit import SIT

if __name__ == "__main__":

    config = {
        "hdl": "pyrtl",
        "config": {
            "module_name": "ram",
            "lib": "pyrtlsock",
            "desc": "Demonstration of a PyRTL hardware simulation in SST",
            "module_dir": "../tests/pyrtl/",
        },
        "ports": {
            "input": [
                {"name": "address", "type": "int", "len": 8},
                {"name": "cs", "type": "int", "len": 1},
                {"name": "we", "type": "int", "len": 1},
                {"name": "oe", "type": "int", "len": 1},
                {"name": "data_in", "type": "int", "len": 8},
            ],
            "output": [{"name": "data_out", "type": "bit", "len": 8}],
        },
    }

    sit_obj = SIT(config)
    sit_obj.generate_boilerplate()
