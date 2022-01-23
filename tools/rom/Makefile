CXX ?= g++

DEFINES := -DCWD=\"$(shell pwd)\"

mext.o: src/json.hpp src/sm64.hpp src/mext.cpp
	$(CXX) -std=c++17 -fexceptions -Iinclude/ -O3 -I. src/mext.cpp $(DEFINES) -o mext

clean:
	rm -f mext

.PHONY: clean
