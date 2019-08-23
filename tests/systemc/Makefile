MAKEFLAGS += --no-print-directory
_CXX ?= /usr/bin/clang++

all: socks zmq

.PHONY: socks
socks:
	cd build && cmake -DCMAKE_CXX_COMPILER=${_CXX} -DIPC=SOCKS .. && $(MAKE)
	sst-register proto proto_LIBDIR=$(CURDIR)/build/socks
	cd build/socks && sst ../../socks/run.py

.PHONY: zmq
zmq:
	cd build && cmake -DCMAKE_CXX_COMPILER=${_CXX} -DIPC=ZMQ .. && $(MAKE)
	sst-register proto proto_LIBDIR=$(CURDIR)/build/zmq
	cd build/zmq && sst ../../zmq/run.py
