CXX      ?= g++
CXXFLAGS ?= -std=c++11 -O2

all: Tree/bst R-Tree/rtree

Tree/bst: Tree/main.cc Tree/arbol.h Tree/nodo.h
	$(CXX) $(CXXFLAGS) -o $@ Tree/main.cc

R-Tree/rtree: R-Tree/main.cpp
	$(CXX) $(CXXFLAGS) -o $@ R-Tree/main.cpp

# Runs both programs; the R-tree reads a scripted session from examples/.
demo: all
	./Tree/bst
	./R-Tree/rtree < examples/rtree-session.txt

clean:
	rm -f Tree/bst R-Tree/rtree

.PHONY: all demo clean
