import pyrtl
import os
import sys

# Create and connect to named pipes
os.mkfifo(str(sys.argv[1]))
os.mkfifo(str(sys.argv[2]))

outFifo = os.open(str(sys.argv[2]), os.O_WRONLY)
inFifo = os.open(str(sys.argv[1]), os.O_RDONLY)

# Inputs and outputs of the module
load = pyrtl.Input(1, 'load')  # using load as reset
startGreen = pyrtl.Input(1, 'startGreen')
greenTime = pyrtl.Input(8, 'greenTime')
yellowTime = pyrtl.Input(8, 'yellowTime')
redTime = pyrtl.Input(8, 'redTime')
out = pyrtl.Output(2, 'out')

# Register the times for each color when load signal is 1
gt = pyrtl.Register(8, 'gt')
yt = pyrtl.Register(8, 'yt')
rt = pyrtl.Register(8, 'rt')
with pyrtl.conditional_assignment:
    with load:
        gt.next |= greenTime - 1
        yt.next |= yellowTime - 1
        rt.next |= redTime - 1

# Registers for state and a counter
counter = pyrtl.Register(8, 'counter')
state = pyrtl.Register(2, 'state')
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
                counter.next = counter + 1

        with state == GREEN:
            with counter == gt:
                state.next |= YELLOW
                counter.next |= 0
            with pyrtl.otherwise:
                counter.next = counter + 1

        with state == YELLOW:
            with counter == yt:
                state.next |= RED
                counter.next |= 0
            with pyrtl.otherwise:
                counter.next = counter + 1
# Output the state
out <<= state
# Setup the simulation
sim_trace = pyrtl.SimulationTrace([load, startGreen, counter, out])
sim = pyrtl.FastSimulation(tracer=sim_trace)

# PyRTL only test
# sim.step({'load': 1, 'startGreen' : 0, 'greenTime': 6, 'yellowTime': 2, 'redTime': 8})
# for i in range(20):
#	sim.step({'load': 0, 'startGreen' : 0, 'greenTime': 0, 'yellowTime': 0, 'redTime': 0})
# sim.step({'load': 1, 'startGreen' : 1, 'greenTime': 5, 'yellowTime': 1, 'redTime': 6})
# for i in range(15):
#	sim.step({'load': 0, 'startGreen' : 1, 'greenTime': 0, 'yellowTime': 0, 'redTime': 0})

# Connect the PyRTL simulation to SST through named pipes
# Step the PyRTL simulation everytime a command is receieved through the input pipe
# then write the output back through the output pipe for SST to handle
# Run until receive a 'q' from the named pipe
while True:
    cmd = os.read(inFifo, 8)
    if cmd != '':
        if cmd == 'q':
            break
        l = int(cmd[0:1], 2)
        s = int(cmd[1:2], 2)
        g = int(cmd[2:4], 16)
        y = int(cmd[4:6], 16)
        r = int(cmd[6:8], 16)
        sim.step({'load': l, 'startGreen': s, 'greenTime': g, 'yellowTime': y, 'redTime': r})
        # print(sim.inspect(out))
        # print(sim.inspect(gt))

        os.write(outFifo, str(sim.inspect(out)))

# Clean up fifos and print out waveform
os.close(inFifo)
os.close(outFifo)
os.remove(str(sys.argv[1]))
os.remove(str(sys.argv[2]))
# os.system('rm ' + str(sys.argv[1]) + ' ' + str(sys.argv[2]))
# sim_trace.render_trace()

# Extra features to print out dot graph, and output verilog
# Dot graph
# with open('intersection.dot', 'w') as fp:
#	pyrtl.output_to_graphviz(fp)
# Verilog
# with open('intersection.v', 'w') as fp:
#	pyrtl.OutputToVerilog(fp)
# Synthesizable verilog
# with open('intersectionSynth.v', 'w') as fp:
#	pyrtl.synthesize()
#	pyrtl.OutputToVerilog(fp)
# Optimized verilog
# with open('intersectionOpt.v', 'w') as fp:
#	pyrtl.optimize()
#	pyrtl.OutputToVerilog(fp)

exit(0)
