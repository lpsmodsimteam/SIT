#!/usr/bin/env python
# -*- coding: utf-8 -*-

from random import SystemRandom
from string import ascii_uppercase, digits
import os

import sst

BASE_PATH = os.getcwd()

CLOCK = "1Hz"
SEED = 0

LINK_DELAY = "1ps"


def get_rand_tmp():
    return "/tmp/" + ''.join(
        SystemRandom().choice(ascii_uppercase + digits) for _ in range(8)
    )


# SystemC
plague_main = sst.Component(
    "Strain Statistics", "plague.strain")
plague_main.addParams({
    "CLOCK": CLOCK,
    "SEED0": 15554,
    "SEED1": 55465,
    "SEED2": 84568,
    "SEED3": 99999,
    "SEED4": 58465,
    "SEED5": 55465,
    "SEED6": 84568,
    "SEED7": 99999,
    "SEED8": 58465
})

limit_comp = sst.Component(
    "Limit Component (SystemC)", "plague.rng")
limit_comp.addParams({
    "clock": CLOCK,
    "proc": os.path.join(BASE_PATH, "rng.o"),
    "ipc_port": get_rand_tmp(),
})

severity_comp = sst.Component(
    "Severity Component (SystemC)", "plague.randf")
severity_comp.addParams({
    "clock": CLOCK,
    "proc": os.path.join(BASE_PATH, "randf.o"),
    "ipc_port": get_rand_tmp(),
})

infectivity_comp = sst.Component(
    "Infectivity Component (SystemC)", "plague.randf")
infectivity_comp.addParams({
    "clock": CLOCK,
    "proc": os.path.join(BASE_PATH, "randf.o"),
    "ipc_port": get_rand_tmp(),
})

lethality_comp = sst.Component(
    "Lethality Component (SystemC)", "plague.randf")
lethality_comp.addParams({
    "clock": CLOCK,
    "proc": os.path.join(BASE_PATH, "randf.o"),
    "ipc_port": get_rand_tmp(),
})

birth_rate_comp = sst.Component(
    "Birth Rate Component (SystemC)", "plague.randf")
birth_rate_comp.addParams({
    "clock": CLOCK,
    "proc": os.path.join(BASE_PATH, "randf.o"),
    "ipc_port": get_rand_tmp(),
})

pthresh_sc_ceil_comp = sst.Component(
    "Ceiling Component (SystemC)", "plague.sc_ceil")
pthresh_sc_ceil_comp.addParams({
    "clock": CLOCK,
    "proc": os.path.join(BASE_PATH, "sc_ceil.o"),
    "ipc_port": get_rand_tmp(),
})

minf_lethality_comp = sst.Component(
    "Minimum Float Lethality Component (SystemC)", "plague.minf")
minf_lethality_comp.addParams({
    "clock": CLOCK,
    "proc": os.path.join(BASE_PATH, "minf.o"),
    "ipc_port": get_rand_tmp(),
})

minf_infectivity_comp = sst.Component(
    "Minimum Float Infectivity Component (SystemC)", "plague.minf")
minf_infectivity_comp.addParams({
    "clock": CLOCK,
    "proc": os.path.join(BASE_PATH, "minf.o"),
    "ipc_port": get_rand_tmp(),
})

# Chisel components
ram_comp = sst.Component(
    "Memory Component (Chisel)", "plague.ram")
ram_comp.addParams({
    "clock": CLOCK,
    "proc": "test:runMain ram.ramMain",
    "ipc_port": get_rand_tmp(),
})

# # PyRTL
# pyrtl_main = sst.Component(
#     "Traffic Light (PyRTL)", "plague.traffic_light_pyrtl")
# pyrtl_main.addParams({
#     "CLOCK": CLOCK,
# })
# pyrtl_comp = sst.Component(
#     "PyRTL Component", "plague.pyrtl_fsm")
# pyrtl_comp.addParams({
#     "clock": CLOCK,
#     "proc": os.path.join(BASE_PATH, "../pyrtl/blackboxes/pyrtl_fsm_driver.py"),
#     "ipc_port": get_rand_tmp(),
# })

# # Plague Simulation main driver
# plague = sst.Component("plague", "plague.plague")
# plague.addParams({
#     "CLOCK": CLOCK,
#     "SEED": SEED,
# })

# connect the subcomponents
sst.Link("limit_din").connect(
    (limit_comp, "rng_din", LINK_DELAY),
    (plague_main, "limit_din", LINK_DELAY)
)
sst.Link("limit_dout").connect(
    (limit_comp, "rng_dout", LINK_DELAY),
    (plague_main, "limit_dout", LINK_DELAY)
)


sst.Link("severity_din").connect(
    (severity_comp, "randf_din", LINK_DELAY),
    (plague_main, "severity_din", LINK_DELAY)
)
sst.Link("severity_dout").connect(
    (severity_comp, "randf_dout", LINK_DELAY),
    (plague_main, "severity_dout", LINK_DELAY)
)

sst.Link("infectivity_din").connect(
    (infectivity_comp, "randf_din", LINK_DELAY),
    (plague_main, "infectivity_din", LINK_DELAY)
)
sst.Link("infectivity_dout").connect(
    (infectivity_comp, "randf_dout", LINK_DELAY),
    (plague_main, "infectivity_dout", LINK_DELAY)
)

sst.Link("lethality_din").connect(
    (lethality_comp, "randf_din", LINK_DELAY),
    (plague_main, "lethality_din", LINK_DELAY)
)
sst.Link("lethality_dout").connect(
    (lethality_comp, "randf_dout", LINK_DELAY),
    (plague_main, "lethality_dout", LINK_DELAY)
)

sst.Link("birth_rate_din").connect(
    (birth_rate_comp, "randf_din", LINK_DELAY),
    (plague_main, "birth_rate_din", LINK_DELAY)
)
sst.Link("birth_rate_dout").connect(
    (birth_rate_comp, "randf_dout", LINK_DELAY),
    (plague_main, "birth_rate_dout", LINK_DELAY)
)

sst.Link("minf_lethality_din").connect(
    (minf_lethality_comp, "minf_din", LINK_DELAY),
    (plague_main, "minf_lethality_din", LINK_DELAY)
)
sst.Link("minf_lethality_dout").connect(
    (minf_lethality_comp, "minf_dout", LINK_DELAY),
    (plague_main, "minf_lethality_dout", LINK_DELAY)
)

sst.Link("minf_infectivity_din").connect(
    (minf_infectivity_comp, "minf_din", LINK_DELAY),
    (plague_main, "minf_infectivity_din", LINK_DELAY)
)
sst.Link("minf_infectivity_dout").connect(
    (minf_infectivity_comp, "minf_dout", LINK_DELAY),
    (plague_main, "minf_infectivity_dout", LINK_DELAY)
)

sst.Link("pthresh_sc_ceil_din").connect(
    (pthresh_sc_ceil_comp, "sc_ceil_din", LINK_DELAY),
    (plague_main, "pthresh_sc_ceil_din", LINK_DELAY)
)
sst.Link("pthresh_sc_ceil_dout").connect(
    (pthresh_sc_ceil_comp, "sc_ceil_dout", LINK_DELAY),
    (plague_main, "pthresh_sc_ceil_dout", LINK_DELAY)
)

sst.Link("ram_din").connect(
    (ram_comp, "ram_din", LINK_DELAY),
    (plague_main, "ram_din", LINK_DELAY)
)
sst.Link("ram_dout").connect(
    (ram_comp, "ram_dout", LINK_DELAY),
    (plague_main, "ram_dout", LINK_DELAY)
)

# sst.Link("py_din").connect(
#     (pyrtl_comp, "pyrtl_fsm_din", LINK_DELAY),
#     (pyrtl_main, "py_din", LINK_DELAY)
# )
# sst.Link("py_dout").connect(
#     (pyrtl_comp, "pyrtl_fsm_dout", LINK_DELAY),
#     (pyrtl_main, "py_dout", LINK_DELAY)
# )
# sst.Link("light1").connect(
#     (plague, "light1", LINK_DELAY),
#     (pyrtl_main, "light_state", LINK_DELAY)
# )

# sst.Link("cars0").connect(
#     (plague, "cars0", LINK_DELAY),
#     (car_generator0, "is_car", LINK_DELAY)
# )
# sst.Link("cars1").connect(
#     (plague, "cars1", LINK_DELAY),
#     (car_generator1, "is_car", LINK_DELAY)
# )

sst.setProgramOption("stopAtCycle", "20s")
