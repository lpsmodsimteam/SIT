import os
import socket
import sys

import pyrtl

socketAddress = sys.argv[1]
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
sock.connect(socketAddress)
sock.sendall(str(os.getpid()).encode())

# Inputs and outputs of the module
load = pyrtl.Input(1, "load")  # using load as reset
startGreen = pyrtl.Input(1, "STARTGREEN")
greenTime = pyrtl.Input(8, "GREENTIME")
yellowTime = pyrtl.Input(8, "YELLOWTIME")
redTime = pyrtl.Input(8, "REDTIME")
out = pyrtl.Output(2, "out")

# Register the times for each color when load signal is 1
gt = pyrtl.Register(8, "gt")
yt = pyrtl.Register(8, "yt")
rt = pyrtl.Register(8, "rt")
with pyrtl.conditional_assignment:
    with load:
        gt.next |= greenTime - 1
        yt.next |= yellowTime - 1
        rt.next |= redTime - 1

# Registers for state and a counter
counter = pyrtl.Register(8, "counter")
state = pyrtl.Register(2, "state")
# enumerate a set of constant to serve as our states
GREEN, YELLOW, RED = [pyrtl.Const(x, bitwidth=2) for x in range(3)]

# PyRTL state machine
with pyrtl.conditional_assignment:
    # Using load as the reset signal
    with load:
        counter.next |= 0
        # 2 entry points into the state machine
        with startGreen:
            state.next |= GREEN
        with pyrtl.otherwise:
            state.next |= RED

    # Main state machine, count until the appropriate time limit for each state
    with pyrtl.otherwise:
        with state == RED:
            with counter == rt:
                state.next |= GREEN
                counter.next |= 0
            with pyrtl.otherwise:
                counter.next |= counter + 1

        with state == GREEN:
            with counter == gt:
                state.next |= YELLOW
                counter.next |= 0
            with pyrtl.otherwise:
                counter.next |= counter + 1

        with state == YELLOW:
            with counter == yt:
                state.next |= RED
                counter.next |= 0
            with pyrtl.otherwise:
                counter.next |= counter + 1
# Output the state
out <<= state

# Setup the simulation
sim_trace = pyrtl.SimulationTrace([load, startGreen, counter, out])
sim = pyrtl.FastSimulation(tracer=sim_trace)

# Connect the PyRTL simulation to SST through named pipes
# Step the PyRTL simulation everytime a command is receieved through the input pipe
# then write the output back through the output pipe for SST to handle
# Run until receive a "q" from the named pipe
while True:
    # cmd = os.read(inFifo, 8).decode("utf-8")
    cmd = str(sock.recv(8).decode("utf-8"))

    if cmd:
        if cmd == "q":
            break
        sim.step({
            "load": int(cmd[0]),
            "STARTGREEN": int(cmd[1]),
            "GREENTIME": int(cmd[2:4]),
            "YELLOWTIME": int(cmd[4:6]),
            "REDTIME": int(cmd[6:8])
        })
        sock.sendall(str(sim.inspect(out)).encode())
