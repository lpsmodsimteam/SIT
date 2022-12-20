from os import environ
from pathlib import Path
from random import SystemRandom
from string import ascii_uppercase, digits

import sst

BASE_PATH = Path.cwd()
CLOCK = "1MHz"
LINK_SPEED = "1ps"


def get_rand_tmp():
    return "".join(
        SystemRandom().choice(ascii_uppercase + digits) for _ in range(8)
    )


def setup(hdl, ipc, comp):
    if ipc == "sock":
        port_prefix = "/tmp/"
    elif ipc == "zmq":
        port_prefix = "ipc:///tmp/"
    else:
        raise NotImplementedError()

    sst.setProgramOption("stopAtCycle", "25us")

    main_comp = sst.Component(comp, f"{hdl}{ipc}.{comp}")
    main_comp.addParams(
        {
            "clock": CLOCK,
        }
    )

    ram_comp = sst.Component("ram", f"{hdl}{ipc}.ram")

    if hdl == "systemc":
        proc = BASE_PATH / "ram.o"
    elif hdl == "pyrtl":
        proc = BASE_PATH / "gen" / "ram_driver.py"
    elif hdl == "verilog":
        import json

        with open("config-cmd.json") as cmd_file:
            data = json.load(cmd_file)
        for k, v in data.items():
            if k != "FILE":
                environ[k] = v
            else:
                proc = v
    else:
        raise NotImplementedError()

    ram_comp.addParams(
        {
            "proc": proc,
            "ipc_port": port_prefix + get_rand_tmp(),
            "clock": CLOCK,
        }
    )

    sst.Link("ram_din").connect(
        (ram_comp, "ram_din", LINK_SPEED),
        (main_comp, "ram_din", LINK_SPEED),
    )
    sst.Link("ram_dout").connect(
        (ram_comp, "ram_dout", LINK_SPEED),
        (main_comp, "ram_dout", LINK_SPEED),
    )
