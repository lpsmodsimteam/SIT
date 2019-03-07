_CXX ?= /usr/bin/clang++

compile:
	cd build && cmake -DCMAKE_CXX_COMPILER=${_CXX} .. && $(MAKE)
	sst-register proto proto_LIBDIR=$(CURDIR)/build


run:
	cd build && sst ../tests/${TEST}

clean:
	# remove all compiled objects
	rm -rf build/*
	# remove cached cmake file
	rm -rf cmake-build-debug
	# sst-register -u proto
