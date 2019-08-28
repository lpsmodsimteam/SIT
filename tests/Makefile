MAKEFLAGS += --no-print-directory
IPC ?= all

.PHONY: test
test: systemc clean


.PHONY: systemc
systemc:
	@echo "\e[1;32mRunning SystemC tests...\e[0m"
	mkdir -p systemc/build
	cd systemc && $(MAKE) ${IPC}

clean:
	# remove all compiled objects and cached files
	@echo "\e[1;32mRemoving cached files...\e[0m"
	find . -name "*.pyc" -type f -delete
	find . \( -name "__pycache__" -o -name "cmake-build-debug" \) -type d -exec rm -rf {} +
	@echo "\e[1;32mRemoving black boxes and build files...\e[0m"
	find . -name "blackboxes" -type d -exec rm -rf {} +
	find . -name "build" -type d -exec rm -rf {} +
