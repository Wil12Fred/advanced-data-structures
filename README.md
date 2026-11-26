# advanced-data-structures

[![CI](https://github.com/Wil12Fred/advanced-data-structures/actions/workflows/ci.yml/badge.svg)](https://github.com/Wil12Fred/advanced-data-structures/actions/workflows/ci.yml) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

An R-tree (Guttman, quadratic split) over n-dimensional rectangles and a small
binary search tree template, written in C++11 for an advanced data structures
course, with invariant tests that check the trees stay valid after every
insertion and deletion.

## Features

- `R-Tree/`: R-tree with configurable `M` (max entries), `m` (min entries) and
  dimension; insertion with quadratic split, range search, deletion with
  condense-tree and re-insertion, root shrinking, level-by-level printing;
  bulk insert/delete from text files.
- `Tree/`: BST template `arbol<T>` with `insert`, `find`, `remove` and
  `convertToList` (flattens the tree into a chain of left pointers), plus a
  fixed demo.
- Menus, prompts and identifiers are in Spanish (original course code).

## Requirements

A C++11 compiler and `make`.

- Debian/Ubuntu: `sudo apt install g++ make`
- Arch: `sudo pacman -S gcc make`

## Build

```bash
make            # builds Tree/bst and R-Tree/rtree
```

## Run

### R-tree

`./R-Tree/rtree` first asks for `M m dim`, then shows a menu:

```
0. Insertar de archivo.       insert every rectangle in ./objects.txt
1. Insertar manualmente.      insert one rectangle (min point, then max point)
2. Buscar rango.              print the stored rectangles that overlap a query
3. Eliminar elemento manual.  delete one rectangle
4. Imprimir.                  print the tree level by level
5. Eliminar de archivo.       delete every rectangle in ./objects_delete.txt
-1. Salir.                    quit
```

The files are read from the current directory. Each rectangle is `dim`
coordinates of the min point followed by `dim` coordinates of the max point,
whitespace separated. Samples are in `examples/`: `objects.txt` (12 rectangles)
and `objects_delete.txt` (4 of them). A scripted session that loads both:

```bash
cd examples && ../R-Tree/rtree < rtree-files-session.txt
```

`rtree-files-session.txt` uses `M=3, m=1, dim=2`: load `objects.txt`, query
`(0,0)-(4,4)`, print, delete `objects_delete.txt`, query again, print. Real
output after the deletions (menus removed; `imprime: <children> <entries>`
then `<entries> en nivel: <level>`, leaves are level 1):

```
(1,3,),(2,4,)            <- query (0,0)-(4,4): (2,2)-(3,3) is gone
(0,0,),(1,1,)
imprime: 2 2
2 en nivel: 3            <- root
(2,6,),(5,10,)
(0,0,),(9,6,)
imprime: 1 1
1 en nivel: 2
(2,6,),(5,10,)
imprime: 3 3
3 en nivel: 2
(0,0,),(2,4,)
(8,1,),(9,2,)
(0,5,),(6,6,)
...
```

`make demo` runs `Tree/bst` and a manual-entry session
(`examples/rtree-session.txt`).

### Binary search tree

`./Tree/bst` inserts `6 8 10 12` then `1 4 7 10 13`, printing the tree in
pre-order after each insertion, and finally after `convertToList`:

```
6
6 8
...
6 1 4 8 7 10 12 10 13
6 1 8 7 4 10 12 10 13
```

## Tests

```bash
make test
```

- `tests/test_rtree.cpp` — builds trees with several `(M, m, dim)` settings
  from a fixed-seed generator (up to 500 rectangles) and after every insert and
  every delete checks: node fill between `m` and `M` (root excepted), all leaves
  at level 1 and each child one level below its parent, parent pointers, each
  parent rectangle covers its child's rectangles, and the stored set equals the
  reference. Range search is compared against brute force. Ends by emptying
  the tree and reusing it.
- `tests/test_bst.cc` — insert/find against a reference multiset, 400 random
  removals keeping BST order and removing exactly one copy, removal of a node
  with only a left child, `convertToList` producing a chain with every value
  (and not crashing on an empty tree), and `arbol<double>`.
- Golden tests: the full output of `Tree/bst`, `examples/rtree-session.txt`
  and `examples/rtree-files-session.txt` must match `tests/golden/` exactly.

## Project layout

```
R-Tree/main.cpp        R-tree and its interactive menu (single file)
Tree/arbol.h, nodo.h   BST template; Tree/main.cc is the demo
examples/              sample objects files and scripted sessions
tests/                 unit tests and golden outputs
```

## Known limitations

- Memory is never freed: rectangles allocated during splits, searches and
  deletes leak. Fine for a short-lived CLI, not for a library.
- Input is not validated: a non-numeric answer to the menu loops forever, and
  `M < 1` or `m > M/2` is not rejected.
- The R-tree has no header of its own; tests include `main.cpp` with its
  `main` renamed.
- `Tree/` duplicates go to the right subtree; `remove` deletes one copy.

## Background

Coursework for an advanced data structures course at a university: the BST in
2016, the R-tree (first assignment) in 2019.

## License

MIT — see [LICENSE](LICENSE).
