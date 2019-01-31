all: default

default:
	mkdir -p build && cd build && cmake .. && $(MAKE)

run:
	cd build && sst ../tests/${TEST}

clean:
	# remove all compiled objects
	rm -rf build/*
