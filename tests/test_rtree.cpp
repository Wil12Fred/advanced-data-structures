// Unit tests for the R-tree in R-Tree/main.cpp.
// The program is a single file with its own main(); rename it so the RTree
// class can be exercised directly.
#define main rtree_program_main
#include "../R-Tree/main.cpp"
#undef main

#include <algorithm>
#include <cstdlib>
#include <set>
#include <sstream>
#include <string>

static int failures = 0;
#define CHECK(cond, msg)                                                   \
    do {                                                                   \
        if (!(cond)) {                                                     \
            std::cerr << "FAIL: " << msg << " (" #cond ")" << std::endl;  \
            failures++;                                                    \
        }                                                                  \
    } while (0)

typedef vector<ld> Pt;
typedef pair<Pt, Pt> Box;

static unsigned rng_state = 12345u;
static int rnd(int n) { rng_state = rng_state * 1103515245u + 12345u; return (rng_state >> 16) % n; }

static Box randomBox(int dim)
{
    Pt a(dim), b(dim);
    for (int d = 0; d < dim; d++) {
        a[d] = rnd(100);
        b[d] = a[d] + 1 + rnd(10);
    }
    return Box(a, b);
}

static bool boxesOverlap(const Box& q, const Box& r)
{
    for (size_t d = 0; d < q.first.size(); d++) {
        if (r.second[d] < q.first[d]) return false;
        if (r.first[d] > q.second[d]) return false;
    }
    return true;
}

static string boxKey(const Pt& mn, const Pt& mx)
{
    Rectangle r(mn, mx);
    ostringstream os;
    streambuf* old = cout.rdbuf(os.rdbuf());
    r.print();
    cout.rdbuf(old);
    return os.str();
}

// Walks the tree and checks the structural invariants of an R-tree:
//  - every leaf is at level 1 and every child is one level below its parent;
//  - every non-root node holds between m and M entries;
//  - a parent entry's rectangle covers every rectangle in its child;
//  - child back-pointers (m_ant) point at the parent.
// Collects the leaf rectangles into `leaves`.
static void checkNode(RTree& t, Node* n, multiset<string>& leaves, const string& ctx)
{
    CHECK(n->m_rectangles.size() == n->m_children.size(), ctx << ": entries/children size mismatch");
    if (n != t.m_head) {
        CHECK((int)n->m_rectangles.size() >= t.m_m, ctx << ": underfull node (" << n->m_rectangles.size() << ")");
    }
    CHECK((int)n->m_rectangles.size() <= t.m_M, ctx << ": overfull node (" << n->m_rectangles.size() << ")");
    bool leaf = n->isLeaf();
    CHECK(leaf == (n->m_level == 1), ctx << ": leaf/level mismatch at level " << n->m_level);
    for (size_t i = 0; i < n->m_rectangles.size(); i++) {
        Rectangle* r = n->m_rectangles[i];
        CHECK(r != 0, ctx << ": null rectangle");
        if (!r) continue;
        if (leaf) {
            leaves.insert(boxKey(r->m_pmin, r->m_pmax));
            continue;
        }
        Node* c = n->m_children[i];
        CHECK(c != 0, ctx << ": null child in internal node");
        if (!c) continue;
        CHECK(c->m_ant == n, ctx << ": child's parent pointer is wrong");
        CHECK(c->m_level == n->m_level - 1, ctx << ": child level not parent level - 1");
        for (size_t j = 0; j < c->m_rectangles.size(); j++) {
            Rectangle* cr = c->m_rectangles[j];
            if (!cr) continue;
            for (int d = 0; d < t.m_dim; d++) {
                CHECK(r->m_pmin[d] <= cr->m_pmin[d] && cr->m_pmax[d] <= r->m_pmax[d],
                      ctx << ": parent rectangle does not cover child rectangle");
            }
        }
        checkNode(t, c, leaves, ctx);
    }
}

static multiset<string> checkTree(RTree& t, const string& ctx)
{
    multiset<string> leaves;
    if (t.m_head) {
        CHECK(t.m_head->m_ant == 0, ctx << ": root has a parent");
        checkNode(t, t.m_head, leaves, ctx);
    }
    return leaves;
}

static multiset<string> findLines(RTree& t, const Box& q)
{
    ostringstream os;
    streambuf* old = cout.rdbuf(os.rdbuf());
    t.Find(new Rectangle(q.first, q.second), t.m_head);
    cout.rdbuf(old);
    multiset<string> out;
    istringstream is(os.str());
    string line;
    while (getline(is, line)) out.insert(line + "\n");
    return out;
}

static multiset<string> expected(const vector<Box>& stored)
{
    multiset<string> s;
    for (size_t i = 0; i < stored.size(); i++) s.insert(boxKey(stored[i].first, stored[i].second));
    return s;
}

static void scenario(int M, int m, int dim, int n)
{
    ostringstream c;
    c << "M=" << M << " m=" << m << " dim=" << dim;
    const string ctx = c.str();
    RTree t(M, m, dim);
    vector<Box> stored;
    for (int i = 0; i < n; i++) {
        Box b = randomBox(dim);
        stored.push_back(b);
        t.Insert(new Rectangle(b.first, b.second), 1);
    }
    CHECK(checkTree(t, ctx + " after insert") == expected(stored), ctx << ": stored set differs after insert");

    for (int q = 0; q < 30; q++) {
        Box qb = randomBox(dim);
        vector<Box> hits;
        for (size_t i = 0; i < stored.size(); i++) if (boxesOverlap(qb, stored[i])) hits.push_back(stored[i]);
        CHECK(findLines(t, qb) == expected(hits), ctx << ": range search differs from brute force");
    }

    // Delete in a scrambled order, checking invariants after every deletion.
    while (!stored.empty()) {
        size_t k = rnd(stored.size());
        Box b = stored[k];
        stored.erase(stored.begin() + k);
        t.Delete(new Rectangle(b.first, b.second));
        CHECK(checkTree(t, ctx + " after delete") == expected(stored),
              ctx << ": stored set differs after deleting, " << stored.size() << " left");
        if (failures) return;
    }
    CHECK(t.m_head == 0, ctx << ": tree not empty after deleting everything");

    // Deleting something absent must be a no-op, and the tree must be reusable.
    Box b = randomBox(dim);
    t.Delete(new Rectangle(b.first, b.second));
    t.Insert(new Rectangle(b.first, b.second), 1);
    stored.push_back(b);
    CHECK(checkTree(t, ctx + " reuse") == expected(stored), ctx << ": tree not reusable after emptying");
}

int main()
{
    scenario(3, 1, 2, 40);
    scenario(4, 2, 2, 200);
    scenario(6, 3, 3, 300);
    scenario(8, 4, 2, 500);
    if (failures) {
        std::cerr << failures << " failure(s)" << std::endl;
        return 1;
    }
    std::cout << "test_rtree: all checks passed" << std::endl;
    return 0;
}
