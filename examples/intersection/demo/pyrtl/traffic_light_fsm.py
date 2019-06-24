import array
import os
import socket
import sys

import msgpack
import pyrtl
# Create and connect to named pipes
# os.mkfifo(str(sys.argv[1]))
# os.mkfifo(str(sys.argv[2]))

# outFifo = os.open(str(sys.argv[2]), os.O_WRONLY)
# inFifo = os.open(str(sys.argv[1]), os.O_RDONLY)

socketAddress = sys.argv[1]
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
sock.connect(socketAddress)

arr = array.array('u', str(os.getpid()))
print(arr)
msg = msgpack.packb(arr.tobytes(), use_bin_type=True)
print("packed", msg)
sock.sendall(msg)
print("sent")


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
    cmd = sock.recv(8).decode("utf-8")
    if cmd:
        if cmd == "q":
            break
        l = int(cmd[0], 2)
        s = int(cmd[1], 2)
        g = int(cmd[2:4], 16)
        y = int(cmd[4:6], 16)
        r = int(cmd[6:8], 16)
        sim.step({"load": l, "startGreen": s, "greenTime": g,
                  "yellowTime": y, "redTime": r})
        # os.write(outFifo, str(sim.inspect(out)).encode())
        sock.sendall(str(sim.inspect(out)).encode())

# Clean up fifos and print out waveform
# os.close(inFifo)
# os.close(outFifo)
# os.system("rm " + str(sys.argv[1]) + " " + str(sys.argv[2]))

exit(0)
