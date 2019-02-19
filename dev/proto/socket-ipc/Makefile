all: default

default:
	mkdir -p build && cd build && cmake .. && $(MAKE)

NPROCS ?= 4
run:
	# execute an SST process using MPI
	cd build && mpirun -np ${NPROCS} sst ../tests/${TEST}

clean:
	# remove all compiled objects
	rm -rf build/*
