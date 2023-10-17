import pyrtl

# Define RAM parameters
depth = 256  # Number of memory locations (2^8 for 8-bit address)
width = 8  # Data width (8 bits)

# Create PyRTL signals for inputs and outputs
address = pyrtl.Input(bitwidth=8, name="address")  # 8-bit address
data_in = pyrtl.Input(bitwidth=8, name="data_in")  # 8-bit data input
cs = pyrtl.Input(bitwidth=1, name="cs")  # Chip select (active low)
oe = pyrtl.Input(bitwidth=1, name="oe")  # Output enable (active low)
we = pyrtl.Input(bitwidth=1, name="we")  # Write enable (active low)
data_out = pyrtl.Output(bitwidth=8, name="data_out")  # 8-bit data output

# Create a synchronous RAM block
ram = pyrtl.MemBlock(bitwidth=8, addrwidth=8, name="ram", asynchronous=False)

# Define the RAM read and write logic
with pyrtl.conditional_assignment:
    with cs:
        with oe:
            data_out <<= ram[address]
        with we:
            ram[address] <<= data_in

# Simulate the PyRTL design
sim_trace = pyrtl.SimulationTrace()
sim = pyrtl.Simulation(tracer=sim_trace)
