// Unit tests for the binary search tree template in Tree/arbol.h.
#include <iostream>
#include <set>
#include <vector>
using namespace std;
#include "../Tree/arbol.h"

static int failures = 0;
#define CHECK(cond, msg)                                                   \
    do {                                                                   \
        if (!(cond)) {                                                     \
            cerr << "FAIL: " << msg << " (" #cond ")" << endl;            \
            failures++;                                                    \
        }                                                                  \
    } while (0)

template <class T>
static void inorder(nodo<T>* n, vector<T>& out)
{
    if (!n) return;
    inorder(n->N[0], out);
    out.push_back(n->dato);
    inorder(n->N[1], out);
}

template <class T>
static multiset<T> contents(arbol<T>& a)
{
    vector<T> v;
    inorder(a.root, v);
    return multiset<T>(v.begin(), v.end());
}

// In-order traversal of a BST (duplicates go right) is non-decreasing.
template <class T>
static bool isSorted(arbol<T>& a)
{
    vector<T> v;
    inorder(a.root, v);
    for (size_t i = 1; i < v.size(); i++)
        if (v[i] < v[i - 1]) return false;
    return true;
}

static unsigned seed = 7u;
static int rnd(int n) { seed = seed * 1103515245u + 12345u; return (seed >> 16) % n; }

static void testInsertFind()
{
    arbol<int> a;
    multiset<int> ref;
    for (int i = 0; i < 300; i++) {
        int x = rnd(100);
        a.insert(x);
        ref.insert(x);
    }
    CHECK(contents(a) == ref, "insert keeps every value");
    CHECK(isSorted(a), "insert keeps BST order");
    for (int x = -5; x < 105; x++) {
        nodo<int>** p;
        bool f = a.find(x, p);
        CHECK(f == (ref.count(x) > 0), "find(" << x << ") agrees with reference");
        if (f) CHECK((*p)->dato == x, "find returns the matching node");
    }
}

static void testRemove()
{
    arbol<int> a;
    multiset<int> ref;
    for (int i = 0; i < 300; i++) {
        int x = rnd(60);
        a.insert(x);
        ref.insert(x);
    }
    for (int i = 0; i < 400; i++) {
        int x = rnd(70);
        a.remove(x);
        multiset<int>::iterator it = ref.find(x);
        if (it != ref.end()) ref.erase(it);
        CHECK(contents(a) == ref, "remove(" << x << ") removes exactly one copy");
        CHECK(isSorted(a), "remove keeps BST order");
        if (failures) return;
    }
}

// The node with only a left child must keep that subtree when removed.
static void testRemoveOnlyLeftChild()
{
    arbol<int> a;
    a.insert(10);
    a.insert(5);
    a.insert(3);
    a.remove(10);
    multiset<int> want;
    want.insert(5);
    want.insert(3);
    CHECK(contents(a) == want, "removing a node with only a left child keeps the left subtree");
}

static void testConvertToList()
{
    arbol<int> a;
    multiset<int> ref;
    for (int i = 0; i < 100; i++) {
        int x = rnd(1000);
        a.insert(x);
        ref.insert(x);
    }
    a.convertToList();
    multiset<int> got;
    int len = 0;
    bool chain = true;
    for (nodo<int>* n = a.root; n; n = n->N[0]) {
        got.insert(n->dato);
        if (n->N[1]) chain = false;
        len++;
    }
    CHECK(chain, "convertToList leaves no right children");
    CHECK(len == 100 && got == ref, "convertToList keeps every value in the chain");

    arbol<int> empty;
    empty.convertToList();  // must not crash on an empty tree
    CHECK(empty.root == 0, "convertToList on an empty tree is a no-op");
}

// arbol<T> must store values of type T, not truncate them to int.
static void testNonIntType()
{
    arbol<double> a;
    a.insert(1.5);
    a.insert(1.25);
    nodo<double>** p;
    CHECK(a.find(1.25, p), "arbol<double> finds 1.25");
    CHECK(a.root && a.root->dato == 1.5, "arbol<double> stores 1.5 unchanged");
}

int main()
{
    testInsertFind();
    testRemove();
    testRemoveOnlyLeftChild();
    testConvertToList();
    testNonIntType();
    if (failures) {
        cerr << failures << " failure(s)" << endl;
        return 1;
    }
    cout << "test_bst: all checks passed" << endl;
    return 0;
}
