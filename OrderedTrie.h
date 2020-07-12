#ifndef ORDERED_TRIE_H
#define ORDERED_TRIE_H

#include <vector>
#include <unordered_map>
#include <iostream>

// data structure for storing collection of sets
// allows efficient checking for existance of subset/superset
template <typename T, typename Alphabet>
class OrderedTrie
{
public:
    // sets are represented as ordered vectors
    typedef std::vector<Alphabet> Set;
private:
    class Node
    {
        std::vector<T> ids;
        std::vector<std::pair<Alphabet,Node>> children;
    public:
        bool empty() const;
        void insert(T id, const Set &value, size_t index);
        void erase(T id, const Set &value, size_t index);
        void findSubsets(const Set &s, size_t index, std::vector<T> &out) const;
        void findSupersets(const Set &s, size_t index, std::vector<T> &out) const;
        // auto-generate non-templated friend function
        friend std::ostream& operator<<(std::ostream &os, const Node &node)
        {
            return os << "<ids=" << node.ids << ", children=" << node.children << ">";
        }
    };

    Node root;
    std::unordered_map<T,Set> valueMap;
public:
    bool contains(T id) const;
    const Set& at(T id) const;
    // insert or update; returns if changes were made
    bool insert(T id, const Set &value);
    // erase if present; returns if changes were made
    bool erase(T id);
    std::vector<T> findSubsets(const Set &s) const;
    std::vector<T> findSupersets(const Set &s) const;
};

// needed for proper linkage
#include "OrderedTrie.cpp"
#endif
