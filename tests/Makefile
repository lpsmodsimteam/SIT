test: generate-test unit-test

generate-test:
	python3 boilerplate/generate_g.py

unit-test:
	cd proto && make install USE_ZMQ=OFF run TEST=socktest.py UNIT_TEST=ON
	cd proto && make install USE_ZMQ=ON run TEST=zmqtest.py UNIT_TEST=ON
