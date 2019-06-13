MAKEFLAGS += --no-print-directory
test_dir ?= unit/build

.PHONY: test_min test
test_min: boilerplate unit clean
test: boilerplate unit unit-extra-ipc clean

.PHONY: boilerplate
boilerplate:
	python3 boilerplate/generate_g.py

.PHONY: unit
unit:
	cd ${test_dir} && cmake -DUNIT_TEST=ON .. && $(MAKE)
	cd ${test_dir} && sst-register proto proto_LIBDIR=${test_dir} && sst ../demo/socks.py

.PHONY: unit-extra-ipc
unit-extra-ipc:
	cd ${test_dir} && cmake -DZMQ=ON -DDEMO_ZMQ=ON -DUNIT_TEST=ON .. && $(MAKE)
	cd ${test_dir} && sst ../demo/zmq.py

clean:
	# remove all compiled objects and cached files
	rm -rf ${test_dir}/* unit/cmake-build-debug
	rm -rf blackboxes
	rm -rf *.pyc __pycache__
