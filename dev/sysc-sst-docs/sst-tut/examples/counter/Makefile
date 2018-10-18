CXX=$(shell sst-config --CXX)
# SST configs
SST_FLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
SST_LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

# SystemC configs
SYSC_HOME = /usr/local/_systemc-sys
SYSC_FLAGS = -I. -I${SYSC_HOME}/include 
SYSC_LDFLAGS =-L. -L${SYSC_HOME}/lib-linux64 -Wl,-rpath=${SYSC_HOME}/lib-linux64 -lsystemc -lm

# define the CPP source files
SRCS = counter.cpp

# define the CPP object files
OBJS = libcounterSST.so

all: ${OBJS} install

${OBJS}: ${SRCS}
	${CXX} ${SST_FLAGS} ${SST_LDFLAGS} ${SYSC_FLAGS} -o $@ $^ ${SYSC_LDFLAGS}

install:
	sst-register counter counter_LIBDIR=$(CURDIR)

clean:
	rm -rf *.o tests/graph ${OBJS}
	sst-register -u counter
