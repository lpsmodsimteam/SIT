# compiler
CXX=clang++

# SST configs
SST_FLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
SST_LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

# SystemC configs
SYSC_HOME = /home/sabbir/projects/sysc-sst/dev/kernels/systemc/systemc-2.3.2/src
SYSC_FLAGS = -I. -I${SYSC_HOME}
SYSC_LDFLAGS =-L. -L${SYSC_HOME} -Wl,-rpath=${SYSC_HOME} -lsystemc -lm

SST_OBJS = libsst_counterSST.so
SST_SRCS = sst_counter.cpp
SYSC_OBJS = counter_module.o
SYSC_SRCS = sysc_counter_tb.cpp

all: ${SST_OBJS} ${SYSC_OBJS}


${SYSC_OBJS}: ${SYSC_SRCS}
	${CXX} -v ${SYSC_FLAGS} -o $@ $^ ${SYSC_LDFLAGS}


${SST_OBJS}: ${SST_SRCS}
	${CXX} -v ${SST_FLAGS} ${SST_LDFLAGS} -o $@ $^
	sst-register counter counter_LIBDIR=$(CURDIR)

clean:
	rm -rf *.o tests/graph ${SST_OBJS}
	sst-register -u sst_counter


${SYSC_OBJS}: ${SYSC_SRCS}
	${CXX} -v ${SST_FLAGS} ${SST_LDFLAGS} ${SYSC_FLAGS} -o $@ $^ ${SYSC_LDFLAGS}
