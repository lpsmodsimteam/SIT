test_dir ?= unit/build
test: boilerplate unit clean

.PHONY: boilerplate
boilerplate:
	python3 boilerplate/generate_g.py

.PHONY: unit
unit:
	cd ${test_dir} && cmake -DUNIT_TEST=ON .. && $(MAKE)
	cd ${test_dir} && sst ../demo/socks.py
	cd ${test_dir} && cmake -DZMQ=ON -DDEMO_ZMQ=ON -DUNIT_TEST=ON .. && $(MAKE)
	cd ${test_dir} && sst ../demo/zmq.py

clean:
	# remove all compiled objects and cached files
	rm -rf ${test_dir}/* unit/cmake-build-debug
	rm -rf blackboxes
	rm -rf *.pyc __pycache__
