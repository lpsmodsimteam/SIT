MAKEFLAGS += --no-print-directory
test_dir ?= systemc/build

.PHONY: test_min test
test_min: boilerplate systemc clean
test: boilerplate systemc unit-extra-ipc clean

.PHONY: boilerplate
boilerplate:
	python3 boilerplate/generate_g.py

.PHONY: systemc
systemc:
	cd ${test_dir} && cmake .. && $(MAKE)

#
#.PHONY: systemc
#systemc:
#	cd ${test_dir} && cmake -DUNIT_TEST=ON .. && $(MAKE)
#	cd ${test_dir} && sst-register proto proto_LIBDIR=${test_dir} && sst ../demo/run.py
#
#.PHONY: systemc-extra-ipc
#systemc-extra-ipc:
#	cd ${test_dir} && cmake -DZMQ=ON -DDEMO_ZMQ=ON -DUNIT_TEST=ON .. && $(MAKE)
#	cd ${test_dir} && sst-register proto proto_LIBDIR=${test_dir} && sst ../demo/zmq.py

clean:
	# remove all compiled objects and cached files
	rm -rf blackboxes
	rm -rf ${test_dir}/*
	find . -name "*.pyc" -type f -delete
	find . \( -name "__pycache__" -o -name "cmake-build-debug" \) -type d -exec rm -rf {} +
