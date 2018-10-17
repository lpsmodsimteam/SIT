CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libcounterSST.so install

libcounterSST.so: counter.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

install:
	sst-register counter counter_LIBDIR=$(CURDIR)

clean:
	rm -rf *.o tests/graph libcounterSST.so
	sst-register -u counter
