CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libintersection.so install

libintersection.so: trafficLight.cpp carGenerator.cpp intersection.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

install:
	sst-register intersection intersection_LIBDIR=$(CURDIR)

clean:
	rm -rf *.o tests/graph libintersection.so
	sst-register -u intersection
