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

define generate
	python $(BASE_DIR)/$1/generate_bbox.py $2 || { $(call eecho,"Install the boilerplate generator"); exit 1; }
endef

define read_mem
	python $(BASE_DIR)/read_mem.py memory_dump.txt && $(call secho,"PASSED: $(HDL) using $1") || $(call eecho,"FAILED: $(HDL) using $1")
endef

HDL_LIST = systemc chisel pyrtl verilog
HDL := $(HDL_LIST)
IPC = sock zmq

.PHONY: generate
generate:
	@$(call generate,$(HDL),$(IPC))


.PHONY: test
test: $(HDL_LIST)


.PHONY: $(HDL_LIST)
$(HDL_LIST):
	@${MAKE} $(IPC) HDL=$@


.PHONY: $(IPC)
.ONESHELL:
$(IPC):
	@if [ "$(HDL)/$@" = "chisel/zmq" ]; then exit; fi
	@$(call iecho,"Running $(HDL) tests using $@...")
	cd $(HDL)
	mkdir -p build
	@$(call iecho,"Generating boilerplate files...")
	@$(call generate,$(HDL),$@) && mv blackboxes $@
	cd build && cmake -DCMAKE_CXX_COMPILER=${CXX} -DIPC=$(shell echo $@ | tr a-z A-Z) .. && $(MAKE)
ifeq ("$(HDL)", "chisel")
	mv ../$@/blackboxes/build.sbt .
	cat <<EOF >> build.sbt
	sourcesInBase := false
	scalaSource in Compile := baseDirectory.value / "../$@"
	unmanagedSourceDirectories in Compile += baseDirectory.value / "../common"
	EOF
endif
ifeq ("$(HDL)", "verilog")
	cp ../$@/blackboxes/ram_driver.py .
	make -f ../Makefile.config dumpconfig
endif
	sst-register $(HDL) $(HDL)_LIBDIR=$(BASE_DIR)/$(HDL)/build/$@
	@$(call iecho,"Running simulations...")
	sst ../$@/run.py || { $(call eecho,"SST failed to launch properly"); exit 1; }
	@$(call read_mem,$@)

.PHONY: ipc
ipc:
	@$(foreach hdl, $(HDL_LIST), ${MAKE} $(IPC) HDL=$(hdl);)


.PHONY: clean
# unregister SST components and remove all compiled objects and cached files
clean:
	@$(foreach hdl, $(HDL_LIST), $(call iecho,"Unregistering $(hdl) components..."); sst-register -u $(hdl);)
	@$(call iecho,"Removing cached files...")
	find . -name "*.pyc" -type f -delete
	find . \( -name "__pycache__" -o -name "cmake-build-debug" \) -type d -exec rm -rf {} +
	@$(call iecho,"Removing black boxes and build files...")
	find . \( -name "blackboxes" -o -name "build" \) -type d -exec rm -rf {} +
	find . \( -name "target" -o -name "test_run_dir" -o -name "project" \) -type d -exec rm -rf {} +
