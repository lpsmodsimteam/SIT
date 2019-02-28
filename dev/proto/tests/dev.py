import os

import sst

galois_lfsr_comp = sst.Component("sst_galois_lfsr", "sst_galois_lfsrSST.sst_galois_lfsr")
sst.setProgramOption("stopAtCycle", "80ns")
BASE_PATH = os.getcwd()
# overide default parameters
galois_lfsr_comp.addParams({
    "proc": os.path.join(BASE_PATH, "galois_lfsr.o"),
})
