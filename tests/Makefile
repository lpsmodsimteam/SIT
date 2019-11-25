MAKEFLAGS += --no-print-directory
BASE_DIR = $(CURDIR)

define iecho
	echo "\e[1;34m$1\e[0m"
endef

define secho
	echo "\e[1;32m$1\e[0m"
endef

define eecho
	echo "\e[1;31m$1\e[0m"
endef

HDL_LIST = systemc pyrtl
IPC = sock zmq

.PHONY: test
test: $(HDL_LIST)


.PHONY: $(HDL_LIST)
$(HDL_LIST):
	@${MAKE} $(IPC) HDL=$@


.PHONY: $(IPC)
.ONESHELL:
$(IPC):
	@$(call iecho,"Running $(HDL) tests using $@...")
	cd $(HDL)
	mkdir -p build
	mkdir -p $@/blackboxes && mkdir -p common/blackboxes
	python generate_bbox.py $@ || { $(call eecho,"Install the boilerplate generator"); exit 1; }
	mv blackboxes/* $@/blackboxes
	cd build && cmake -DCMAKE_CXX_COMPILER=${CXX} -DIPC=$(shell echo $@ | tr a-z A-Z) .. && $(MAKE)
	sst-register $(HDL) $(HDL)_LIBDIR=$(BASE_DIR)/$(HDL)/build/$@
	sst ../$@/run.py || { $(call eecho,"SST failed to launch properly - make sure to clear the cache"); exit 1; }
	python ../../read_mem.py memory_dump.txt && $(call secho,"PASSED: $(HDL) using $@") || $(call eecho,"FAILED: $(HDL) using $@")


.PHONY: clean
# unregister SST components and remove all compiled objects and cached files
clean:
	@$(foreach hdl, $(HDL_LIST), $(call iecho,"Unregistering $(hdl) components..."); sst-register -u $(hdl);)
	@$(call iecho,"Removing cached files...")
	find . -name "*.pyc" -type f -delete
	find . \( -name "__pycache__" -o -name "cmake-build-debug" \) -type d -exec rm -rf {} +
	@$(call iecho,"Removing black boxes and build files...")
	find . \( -name "blackboxes" -o -name "build" \) -type d -exec rm -rf {} +
