#include "OrderedTrie.h"

//----------------- OrderedTrie::Node -----------

bool OrderedTrie::Node::empty() const
{
    return ids.empty() && children.empty();
}

void OrderedTrie::Node::insert(T id, const Set &value, size_t index)
{
    if ( index >= value.size() )
        ids.push_back(id);
    else
    {
        const Alphabet nextElem = value[index];
        // find position to insert
        size_t pos = 0;
        while ( pos < children.size() && children[pos].first < nextElem )
            pos++;
        // ensure existance of suitable child
        if ( pos == children.size() || children[pos].first > nextElem )
            children.insert(pos, pair(nextElem, Node()));
        // insert
        children[pos].second.insert(id, value, index + 1);
    }
}

void OrderedTrie::Node::erase(T id, const Set &value, size_t index)
{
    if ( index >= value.size() )
    {
        // remove id from ids
        size_t pos = 0;
        while ( ids[pos] != id )
            pos++;
        ids[pos] = ids.back();
        ids.pop_back();
    }
    else
    {
        // find containing child
        auto it = children.begin();
        while ( it->first != value[index] )
            ++it;
        // erase from child
        it->second.erase(id, value, index + 1);
        // erase child if now empty
        if ( it->second.empty() )
            children.erase(it);
    }
}

void OrderedTrie::Node::findSubsets(const Set &s, size_t index, std::vector<T> &out) const
{
}

void OrderedTrie::Node::findSupersets(const Set &s, size_t index, std::vector<T> &out) const
{
}

//----------------- OrderedTrie ------------------

void OrderedTrie::insert(T id, const Set &value)
{
    valueMap.insert(id, value);
    root.insert(id, value, 0);
}

void OrderedTrie::update(T id, const Set &value)
{
    Set &storedValue = valueMap[id];
    if ( storedValue != value )
    {
        root.erase(id, storedValue, 0);
        root.insert(id, value, 0);
        storedValue = value;
    }
}

void OrderedTrie::erase(T id)
{
    root.erase(id, valueMap.at(id), 0);
    valueMap.erase(id);
}

std::vector<T> OrderedTrie::findSubsets(const Set &s) const
{
}

std::vector<T> OrderedTrie::findSupersets(const Set &s) const
{
}
