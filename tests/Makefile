test: generate-test unit-test clean

generate-test:
	python3.6 boilerplate/generate_g.py

unit-test:
	cd proto/build && cmake -DZMQ=OFF -DDEMO_ZMQ=OFF -DUNIT_TEST=ON .. && $(MAKE)
	cd proto/build && sst ../demo/socks.py
	cd proto/build && cmake -DZMQ=ON -DDEMO_ZMQ=ON -DUNIT_TEST=ON .. && $(MAKE)
	cd proto/build && sst ../demo/zmq.py

clean:
	# remove all compiled objects and cached files
	rm -rf proto/build/* proto/cmake-build-debug
	rm -rf *.pyc
