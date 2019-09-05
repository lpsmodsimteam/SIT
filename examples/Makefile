MAKEFLAGS += --no-print-directory
_CXX ?= /usr/bin/clang++
HYBRID := OFF

.PHONY: intersection
intersection:
	@echo "\e[1;32mRunning intersection...\e[0m"
	mkdir -p intersection/build
	cd intersection/build && cmake -DCMAKE_CXX_COMPILER=${_CXX} -DHYBRID=${HYBRID} .. && $(MAKE)
	sst-register intersection intersection_LIBDIR=$(CURDIR)/build
ifeq ($(HYBRID),ON)
	cd intersection/build && sst ../pyrtl/run.py
else
	cd intersection/build && sst ../systemc/run.py
endif

.PHONY: clean
clean:
	# remove all compiled objects and cached files
	@echo "\e[1;32mRemoving cached files...\e[0m"
	find . -name "*.pyc" -type f -delete
	find . \( -name "__pycache__" -o -name "cmake-build-debug" \) -type d -exec rm -rf {} +
	@echo "\e[1;32mRemoving black boxes and build files...\e[0m"
	find . -name "build" -type d -exec rm -rf {} +
