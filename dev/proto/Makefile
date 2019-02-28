all: compile

compile:
	mkdir -p build && cd build && cmake .. && $(MAKE)

run:
	# execute an SST process using MPI
	cd build && sst ../tests/${TEST}

clean:
	# remove all compiled objects
	rm -rf build/*
	# remove cached cmake file
	rm -rf cmake-build-debug
