#ifndef INFORMATIVE_GRAPH_H
#define INFORMATIVE_GRAPH_H

#include <vector>
#include <unordered_map>

// data structure for storing collection of sets
// allows efficient checking for existance of subset/superset
template <typename T, typename Alphabet>
class OrderedTrie
{
public:
    // sets are represented as ordered vectors
    typdef std::vector<Alphabet> Set;
private:
    class Node
    {
        std::vector<T> ids;
        std::vector<std::pair<Alphabet,TrieNode>> children;
    public:
        bool empty() const;
        void insert(T id, const Set &value, size_t index);
        void erase(T id, const Set &value, size_t index);
        void findSubsets(const Set &s, size_t index, std::vector<T> &out) const;
        void findSupersets(const Set &s, size_t index, std::vector<T> &out) const;
    };

    Node root;
    std::unordered_map<T,Set> valueMap;
public:
    void insert(T id, const Set &value);
    void update(T id, const Set &value);
    void erase(T id);
    std::vector<T> findSubsets(const Set &s) const;
    std::vector<T> findSupersets(const Set &s) const;
};

#endif
