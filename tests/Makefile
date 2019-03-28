test: generate-test unit-test clean

generate-test:
	python3 boilerplate/generate_g.py

unit-test:
	cd proto && make install USE_ZMQ=OFF run TEST=socktest.py UNIT_TEST=ON
	cd proto && make install USE_ZMQ=ON run TEST=zmqtest.py UNIT_TEST=ON

clean:
	# remove all compiled objects and cached files
	rm -rf proto/build/* proto/cmake-build-debug
	rm -rf *.pyc
