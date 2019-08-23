MAKEFLAGS += --no-print-directory
IPC ?= all

.PHONY: test
test: boilerplate systemc clean

.PHONY: boilerplate
boilerplate:
	python3 boilerplate/generate_g.py

.PHONY: systemc
systemc:
	cd systemc && $(MAKE) ${IPC}

clean:
	# remove all compiled objects and cached files
	rm -rf blackboxes
	rm -rf systemc/build/*
	find . -name "*.pyc" -type f -delete
	find . \( -name "__pycache__" -o -name "cmake-build-debug" \) -type d -exec rm -rf {} +
