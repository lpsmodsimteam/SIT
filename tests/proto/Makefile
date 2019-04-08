_CXX ?= /usr/bin/clang++
USE_ZMQ ?= OFF
UNIT_TEST ?= OFF
SIM ?= ON

install:
	cd build && cmake -DCMAKE_CXX_COMPILER=${_CXX} -DZMQ=${USE_ZMQ} -DDEMO_ZMQ=${USE_ZMQ} -DUNIT_TEST=${UNIT_TEST} -DSIM=${SIM} .. && $(MAKE)
	sst-register proto proto_LIBDIR=$(CURDIR)/build

run:
	cd build && sst ../demo/${TEST}

clean:
	# remove all compiled objects and cached cmake files
	rm -rf build/* cmake-build-debug
