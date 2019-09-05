MAKEFLAGS += --no-print-directory
_CXX ?= /usr/bin/clang++
HYBRID := OFF

.PHONY: generate
generate:
	python3 intersection/generate_bbox.py
	mv blackboxes intersection/blackboxes

.PHONY: intersection
intersection:
	@echo "\e[1;32mRunning intersection...\e[0m"
	$(MAKE) generate
	mkdir -p intersection/build
	cd intersection/build && cmake -DCMAKE_CXX_COMPILER=${_CXX} -DHYBRID=${HYBRID} .. && $(MAKE)
	sst-register intersection intersection_LIBDIR=$(CURDIR)/build
ifeq ($(HYBRID),ON)
	@echo "\e[1;33mRunning hybrid version...\e[0m"
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
	find . -name "blackboxes" -type d -exec rm -rf {} +
	find . -name "build" -type d -exec rm -rf {} +
