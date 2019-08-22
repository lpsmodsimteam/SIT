_CXX ?= /usr/bin/clang++
USE_ZMQ ?= OFF
UNIT_TEST ?= OFF

.PHONE: def
def: socks zmq

.PHONY: socks
socks:
	cd build && cmake -DCMAKE_CXX_COMPILER=${_CXX} -DZMQ=OFF \
	-DSOCKS=ON .. && $(MAKE)
	sst-register proto proto_LIBDIR=$(CURDIR)/build/socks
	cd build/socks && sst ../../socks/run.py

.PHONE: zmq
zmq:
	cd build && cmake -DCMAKE_CXX_COMPILER=${_CXX} -DZMQ=ON \
	-DSOCKS=OFF .. && $(MAKE)
	sst-register proto proto_LIBDIR=$(CURDIR)/build/zmq
