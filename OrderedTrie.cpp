//----------------- OrderedTrie::Node -----------

template <typename T, typename Alphabet>
bool OrderedTrie<T,Alphabet>::Node::empty() const
{
    return ids.empty() && children.empty();
}

template <typename T, typename Alphabet>
void OrderedTrie<T,Alphabet>::Node::insert(T id, const Set &value, size_t index)
{
    if ( index >= value.size() )
        ids.push_back(id);
    else
    {
        const Alphabet nextElem = value[index];
        // find position to insert
        auto child = children.begin();
        while ( child < children.end() && child->first < nextElem )
            child++;
        // ensure existance of suitable child
        if ( child == children.end() || child->first > nextElem )
            child = children.insert(child, std::pair<Alphabet,Node>(nextElem, Node()));
        // insert
        child->second.insert(id, value, index + 1);
    }
}

template <typename T, typename Alphabet>
void OrderedTrie<T,Alphabet>::Node::erase(T id, const Set &value, size_t index)
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
        auto child = children.begin();
        while ( child->first != value[index] )
            ++child;
        // erase from child
        child->second.erase(id, value, index + 1);
        // erase child if now empty
        if ( child->second.empty() )
            children.erase(child);
    }
}

template <typename T, typename Alphabet>
void OrderedTrie<T,Alphabet>::Node::findSubsets(const Set &s, size_t index, std::vector<T> &out) const
{
    // current prefix has been subset of s[0..index]
    for ( T id : ids )
        out.push_back(id);
    // check children
    auto child = children.begin();
    while ( index < s.size() && child < children.end() )
    {
        if ( s[index] < child->first )
            index++;
        else if ( s[index] > child->first )
            child++;
        else
            (child++)->second.findSubsets(s, ++index, out);
    }
}

template <typename T, typename Alphabet>
void OrderedTrie<T,Alphabet>::Node::findSupersets(const Set &s, size_t index, std::vector<T> &out) const
{
    // current prefix has been superset of s[0..index]
    if ( index >= s.size() )
    {
        for ( T id : ids )
            out.push_back(id);
        // all children must be supersets as well
        for ( const std::pair<Alphabet,Node> &child : children )
            child.second.findSupersets(s, index, out);
        return;
    }
    // check children
    for ( const std::pair<Alphabet,Node> &child : children )
    {
        if ( s[index] < child.first )
            break; // as children are sorted, this condition won't change
        else if ( s[index] > child.first )
            child.second.findSupersets(s, index, out);
        else
            child.second.findSupersets(s, ++index, out);
    }
}

//----------------- OrderedTrie ------------------

template <typename T, typename Alphabet>
void OrderedTrie<T,Alphabet>::insert(T id, const Set &value)
{
    valueMap[id] = value;
    root.insert(id, value, 0);
}

template <typename T, typename Alphabet>
void OrderedTrie<T,Alphabet>::update(T id, const Set &value)
{
    Set &storedValue = valueMap[id];
    if ( storedValue != value )
    {
        root.erase(id, storedValue, 0);
        root.insert(id, value, 0);
        storedValue = value;
    }
}

template <typename T, typename Alphabet>
void OrderedTrie<T,Alphabet>::erase(T id)
{
    root.erase(id, valueMap.at(id), 0);
    valueMap.erase(id);
}

template <typename T, typename Alphabet>
std::vector<T> OrderedTrie<T,Alphabet>::findSubsets(const Set &s) const
{
    std::vector<T> out;
    root.findSubsets(s, 0, out);
    return out;
}

template <typename T, typename Alphabet>
std::vector<T> OrderedTrie<T,Alphabet>::findSupersets(const Set &s) const
{
    std::vector<T> out;
    root.findSupersets(s, 0, out);
    return out;
}
