from pathlib import Path
import tempfile

import sst

BASE_PATH = Path.cwd()

with tempfile.NamedTemporaryFile() as temp_fp:

    sst.setProgramOption("stopAtCycle", "25us")

    main_comp = sst.Component("unit", "systemcsock.unit")
    main_comp.addParams(
        {
            "clock": "1MHz",
        }
    )

    ram_comp = sst.Component("ram", "systemcsock.ram")

    ram_comp.addParams(
        {
            "proc": BASE_PATH / "ram.o",
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
