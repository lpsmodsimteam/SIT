# compiler
CXX=clang++

# SST configs
SST_FLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
SST_LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

# SystemC configs
SYSC_FLAGS = -I. -I${SYSC_HOME}/include
SYSC_LDFLAGS =-L. -L${SYSC_HOME}/lib-linux64 -Wl,-rpath,${SYSC_HOME}/lib-linux64
CXX_FLAGS = -lsystemc -lm

SST_OBJS = libsst_counterSST.so
SST_SRCS = sst_counter.cpp
SYSC_OBJS = sysc_counter_module.o
SYSC_SRCS = sysc_counter.cpp sysc_counter_tb.cpp

all: ${SYSC_OBJS} ${SST_OBJS}

# define the CPP object files
OBJS = ${SYSC_SRCS:.cpp=.o}
${SYSC_OBJS}: ${OBJS}
	${CXX} ${SYSC_FLAGS} ${SYSC_LDFLAGS} -o ${SYSC_OBJS} ${OBJS} ${CXX_FLAGS}

.cpp.o:
	${CXX} ${SYSC_FLAGS} -c $< -o $@

${SST_OBJS}: ${SST_SRCS}
	${CXX} -v ${SST_FLAGS} ${SST_LDFLAGS} -o $@ $^
	sst-register counter counter_LIBDIR=$(CURDIR)

clean:
	find . \( -name "*.o" -o -name ${SST_OBJS} -o -name ${SYSC_OBJS} \) -type f -delete
	sst-register -u sst_counter
