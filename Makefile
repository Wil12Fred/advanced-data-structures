CXX      ?= g++
CXXFLAGS ?= -std=c++11 -O2

all: Tree/bst R-Tree/rtree

Tree/bst: Tree/main.cc Tree/arbol.h Tree/nodo.h
	$(CXX) $(CXXFLAGS) -o $@ Tree/main.cc

R-Tree/rtree: R-Tree/main.cpp
	$(CXX) $(CXXFLAGS) -o $@ R-Tree/main.cpp

tests/test_bst: tests/test_bst.cc Tree/arbol.h Tree/nodo.h
	$(CXX) $(CXXFLAGS) -o $@ tests/test_bst.cc

tests/test_rtree: tests/test_rtree.cpp R-Tree/main.cpp
	$(CXX) $(CXXFLAGS) -o $@ tests/test_rtree.cpp

# Runs both programs; the R-tree reads a scripted session from examples/.
demo: all
	./Tree/bst
	./R-Tree/rtree < examples/rtree-session.txt

# Unit tests (invariants) plus golden tests: each program's full output on a
# committed session must match tests/golden/ byte for byte. The file-driven
# session runs inside examples/, where objects.txt and objects_delete.txt live.
test: all tests/test_bst tests/test_rtree
	./tests/test_bst
	./tests/test_rtree
	./Tree/bst | diff -u tests/golden/bst.out -
	./R-Tree/rtree < examples/rtree-session.txt | diff -u tests/golden/rtree-session.out -
	cd examples && ../R-Tree/rtree < rtree-files-session.txt | diff -u ../tests/golden/rtree-files-session.out -
	@echo "all tests passed"

clean:
	rm -f Tree/bst R-Tree/rtree tests/test_bst tests/test_rtree

.PHONY: all demo test clean
