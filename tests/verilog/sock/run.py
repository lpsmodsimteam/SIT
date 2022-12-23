import json
from os import environ
import tempfile

import sst


with tempfile.NamedTemporaryFile() as temp_fp:

    sst.setProgramOption("stopAtCycle", "25us")

    main_comp = sst.Component("unit", "verilogsock.unit")
    main_comp.addParams(
        {
            "clock": "1MHz",
        }
    )

    ram_comp = sst.Component("ram", "verilogsock.ram")

    proc = ""
    with open("config-cmd.json") as cmd_file:
        data = json.load(cmd_file)
    for k, v in data.items():
        if k != "FILE":
            environ[k] = v
        else:
            proc = v

    ram_comp.addParams(
        {
            "proc": proc,
            "ipc_port": temp_fp.name,
            "clock": "1MHz",
        }
    )

    sst.Link("ram_din").connect(
        (ram_comp, "ram_din", "1ps"),
        (main_comp, "ram_din", "1ps"),
    )
    sst.Link("ram_dout").connect(
        (ram_comp, "ram_dout", "1ps"),
        (main_comp, "ram_dout", "1ps"),
    )
