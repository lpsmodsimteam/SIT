import pyrtl

chance = pyrtl.Input(4, "chance")
gene = pyrtl.Input(4, "gene")
out = pyrtl.Output(2, "out")

circle = pyrtl.Input(32, "circle")
square = pyrtl.Input(32, "square")

mutation_state = pyrtl.Register(2, "mutation_state")
NEUTRAL, GOOD, BAD = [pyrtl.Const(x, bitwidth=2) for x in range(3)]
with pyrtl.conditional_assignment:
    with chance == gene:
        mutation_state.next |= GOOD
    with chance == gene - 1:
        mutation_state.next |= BAD
    with pyrtl.otherwise:
        mutation_state.next |= NEUTRAL

out <<= mutation_state

# Setup the simulation
sim_trace = pyrtl.SimulationTrace([chance, gene, out])
sim = pyrtl.FastSimulation(tracer=sim_trace)
