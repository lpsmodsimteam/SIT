_CXX ?= /usr/bin/clang++

compile:
	cd build && cmake -DCMAKE_CXX_COMPILER=${_CXX} .. && $(MAKE)

run:
	# execute an SST process using MPI
	cd build && sst ../tests/${TEST}

clean:
	# remove all compiled objects
	rm -rf build/*
	# remove cached cmake file
	rm -rf cmake-build-debug
