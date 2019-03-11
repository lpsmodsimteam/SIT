_CXX ?= /usr/bin/clang++
USE_ZMQ ?= OFF

compile:
	cd build && cmake -DCMAKE_CXX_COMPILER=${_CXX} -DZMQ=${USE_ZMQ} .. && $(MAKE)
	sst-register proto proto_LIBDIR=$(CURDIR)/build


run:
	cd build && sst ../tests/${TEST}

clean:
	# remove all compiled objects and cached cmake files
	rm -rf build/* cmake-build-debug
