MAKEFLAGS += --no-print-directory
BASE_DIR = $(CURDIR)

HDL_LIST = systemc pyrtl
IPC = sock zmq

.PHONY: test
test: $(HDL_LIST)


.PHONY: $(HDL_LIST)
$(HDL_LIST):
	${MAKE} $(IPC) HDL=$@


.PHONY: $(IPC)
.ONESHELL:
$(IPC):
	@echo "\e[1;34mRunning $(HDL) tests...\e[0m"
	cd $(HDL)
	mkdir -p build
	@echo "\e[1;34mUsing IPC: $@...\e[0m"
	mkdir -p $@/blackboxes && mkdir -p common/blackboxes
	python generate_bbox.py $@
	mv blackboxes/* $@/blackboxes
	cd build && cmake -DCMAKE_CXX_COMPILER=${CXX} -DIPC=$(shell echo $@ | tr a-z A-Z) .. && $(MAKE)
	sst-register $(HDL) $(HDL)_LIBDIR=$(BASE_DIR)/$(HDL)/build/$@
	sst ../$@/run.py
	python ../../read_mem.py memory_dump.txt


.PHONY: clean
# unregister SST components and remove all compiled objects and cached files
clean:
	@$(foreach hdl, $(HDL_LIST), echo "\e[1;32mUnregistering $(hdl) components...\e[0m"; sst-register -u $(hdl);)
	@echo "\e[1;32mRemoving cached files...\e[0m"
	find . -name "*.pyc" -type f -delete
	find . \( -name "__pycache__" -o -name "cmake-build-debug" \) -type d -exec rm -rf {} +
	@echo "\e[1;32mRemoving black boxes and build files...\e[0m"
	find . \( -name "blackboxes" -o -name "build" \) -type d -exec rm -rf {} +
