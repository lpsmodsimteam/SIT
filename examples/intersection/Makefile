_CXX ?= /usr/bin/clang++
USE_ZMQ ?= OFF
PYRTL ?= OFF

install:
	cd build && cmake -DCMAKE_CXX_COMPILER=${_CXX} -DZMQ=${USE_ZMQ} \
	-DPYRTL=${PYRTL} -DDRIVER=${DRIVER} .. && $(MAKE)
	sst-register proto proto_LIBDIR=$(CURDIR)/build

run:
	cd build && sst ../demo/${TEST}

clean:
	# remove all compiled objects and cached cmake files
	rm -rf build/* cmake-build-debug
