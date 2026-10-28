#include "AVL.h"
#include <cassert>
#include <cmath>
#include <map>
#include <random>
#include <vector>

static void check(BinarySearchTree &t, const std::multimap<Key, Value> &ref)
{
    assert(t.size() == ref.size());
    std::vector<Key> got, want;
    for (auto it = t.begin(); it != t.end(); ++it)
    {
        got.push_back(it->first);
    }
    for (auto &kv : ref)
    {
        want.push_back(kv.first);
    }
    assert(got == want);
    if (!ref.empty())
    {
        assert(t.max_height() <= 1.45 * std::log2(ref.size() + 2) + 1);
        assert(t.min()->first == ref.begin()->first);
        assert(t.max()->first == ref.rbegin()->first);
        auto last = t.end();
        --last;
        assert(last->first == ref.rbegin()->first);
    }
}

int main()
{
    BinarySearchTree t;
    const Key N = 100000;
    for (Key i = 0; i < N; ++i)
    {
        t.insert(i, i);
    }
    std::cout << "sorted insert " << N << " keys: size=" << t.size() << " height=" << t.max_height() << "\n";
    for (Key i = 0; i < N; i += 2)
    {
        t.erase(i);
    }
    assert(t.size() == N / 2 && t.find(3) != t.end() && t.find(4) == t.end());
    std::cout << "erase even keys: size=" << t.size() << " height=" << t.max_height() << "\n";

    std::mt19937 rng(42);
    BinarySearchTree r;
    std::multimap<Key, Value> ref;
    for (int step = 0; step < 20000; ++step)
    {
        Key k = rng() % 500;
        if (rng() % 3)
        {
            r.insert(k, step);
            ref.emplace(k, step);
        }
        else
        {
            r.erase(k);
            ref.erase(k);
        }
        if (step % 1000 == 0)
        {
            check(r, ref);
        }
    }
    check(r, ref);
    Key dup = ref.begin()->first;
    auto range = r.equalRange(dup);
    size_t cnt = 0;
    for (auto it = range.first; it != range.second; ++it)
    {
        ++cnt;
    }
    assert(cnt == ref.count(dup));

    BinarySearchTree copy(r);
    copy.insert(7, 1);
    ref.emplace(7, 1);
    check(copy, ref);
    BinarySearchTree moved(std::move(copy));
    check(moved, ref);
    for (auto &kv : std::map<Key, Value>(ref.begin(), ref.end()))
    {
        moved.erase(kv.first);
    }
    assert(moved.size() == 0 && moved.begin() == moved.end());
    std::cout << "random test vs std::multimap (20000 ops): OK\n";
}
