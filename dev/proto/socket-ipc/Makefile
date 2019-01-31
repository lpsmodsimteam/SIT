all: default

default:
	mkdir -p build && cd build && cmake .. && $(MAKE)

run:
	cd build && sst ../tests/counter.py

clean:
	# remove all compiled objects
	rm -rf build/*
