from random import SystemRandom
from string import ascii_uppercase, digits
import os

import sst

BASE_PATH = os.getcwd()

sst.setProgramOption("stopAtCycle", "80ns")

galois_lfsr_comp = sst.Component("socktest", "socktest.socktest")

# override default parameters
galois_lfsr_comp.addParams({
    "proc": os.path.join(BASE_PATH, "galois_lfsr.o"),
    "ipc_port": "/tmp/" + ''.join(SystemRandom().choice(ascii_uppercase + digits) for _ in range(8)),
    "clock": "500MHz",
})
