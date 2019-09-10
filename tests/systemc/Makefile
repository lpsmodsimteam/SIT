MAKEFLAGS += --no-print-directory
_CXX ?= /usr/bin/clang++
IPC =

all: sock zmq

.PHONY: generate
generate:
	mkdir -p ${IPC}/blackboxes && mkdir -p common/blackboxes
	python3 generate_bbox.py ${IPC}
	mv blackboxes/* ${IPC}/blackboxes

.PHONY: sock
sock:
	@echo "\e[1;34mUsing IPC: Unix domain sockets...\e[0m"
	$(MAKE) generate IPC=sock
	cd build && cmake -DCMAKE_CXX_COMPILER=${_CXX} -DIPC=SOCK .. && $(MAKE)
	sst-register systemc systemc_LIBDIR=$(CURDIR)/build/sock
	cd build/sock && sst ../../sock/run.py
	python3 ../read_mem.py build/sock/memory_dump.txt

.PHONY: zmq
zmq:
	@echo "\e[1;34mUsing IPC: ZeroMQ...\e[0m"
	$(MAKE) generate IPC=zmq
	cd build && cmake -DCMAKE_CXX_COMPILER=${_CXX} -DIPC=ZMQ .. && $(MAKE)
	sst-register systemc systemc_LIBDIR=$(CURDIR)/build/zmq
	cd build/zmq && sst ../../zmq/run.py
	python3 ../read_mem.py build/zmq/memory_dump.txt
