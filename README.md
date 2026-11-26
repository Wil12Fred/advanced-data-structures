# EstructurasDeDatosAvanzadas

Assignments for the *Advanced Data Structures* course at UCSP (Universidad
Católica San Pablo, Peru). Menus, prompts and identifiers are in Spanish.

| Folder | What | Year |
|---|---|---|
| `R-Tree/` | **Assignment 1**: an R-tree (Guttman) over n-dimensional rectangles, with quadratic split, range search, deletion and printing | 2019 |
| `Tree/` | a small binary search tree template (insert, find, remove, `convertToList`) with a fixed demo | 2016 |

## Build and run

Requires a C++11 compiler and `make`.

```bash
make            # builds Tree/bst and R-Tree/rtree
make demo       # runs both, feeding the R-tree a scripted session
```

### R-tree

`./R-Tree/rtree` first asks for `M m dim` (max and min entries per node, and
the number of dimensions), then shows a menu:

```
0. Insertar de archivo.       insert every rectangle in ./objects.txt
1. Insertar manualmente.      insert one rectangle (min point, then max point)
2. Buscar rango.              print the stored rectangles that overlap a query
3. Eliminar elemento manual.  delete one rectangle
4. Imprimir.                  print the tree level by level
5. Eliminar de archivo.       delete every rectangle in ./objects_delete.txt
-1. Salir.                    quit
```

`objects.txt` and `objects_delete.txt` are not included: they are plain
whitespace-separated numbers, `dim` coordinates of the min point followed by
`dim` coordinates of the max point, per rectangle.

`examples/rtree-session.txt` (used by `make demo`) builds a tree with
`M=3, m=1, dim=2`, inserts four rectangles (forcing a split), runs a range
query, prints, deletes one rectangle and prints again. Excerpt of the output,
menus removed:

```
(0,0,),(1,1,)            <- range query (0,0)-(2.5,2.5) hits these two
(2,2,),(3,3,)
imprime: 2 2
2 en nivel: 2            <- root: 2 entries
(0,0,),(3,3,)
(0,5,),(6,6,)
...
imprime: 2 2             <- after deleting (2,2)-(3,3)
2 en nivel: 2
(0,0,),(1,1,)
(0,5,),(6,6,)
```

### Binary search tree

`./Tree/bst` inserts `6 8 10 12` and then `1 4 7 10 13`, printing the tree in
pre-order after each insertion, and finally after `convertToList`:

```
6
6 8
...
6 1 4 8 7 10 12 10 13
6 1 8 7 4 10 12 10 13
```
