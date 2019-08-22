MAKEFLAGS += --no-print-directory
test_dir ?= systemc/build
IPC ?= def

.PHONY: test_min test
test_min: boilerplate systemc clean
test: boilerplate systemc unit-extra-ipc clean

.PHONY: boilerplate
boilerplate:
	python3 boilerplate/generate_g.py

.PHONY: systemc
systemc:
	cd systemc && $(MAKE) ${IPC}

clean:
	# remove all compiled objects and cached files
	rm -rf blackboxes
	rm -rf ${test_dir}/*
	find . -name "*.pyc" -type f -delete
	find . \( -name "__pycache__" -o -name "cmake-build-debug" \) -type d -exec rm -rf {} +
