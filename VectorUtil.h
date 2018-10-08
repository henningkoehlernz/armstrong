#ifndef VECTOR_UTIL_H
#define VECTOR_UTIL_H

#include <bits/stdc++.h>

template <typename T>
static std::vector<T> operator-(const std::vector<T> &v, const T &value)
{
    std::vector<T> result;
    for ( const T &elem : v )
        if ( elem != value )
            result.push_back(elem);
    return result;
}

template <typename T>
static bool contains(const std::vector<T> &v, const T &value)
{
    for ( const T &elem : v )
        if ( elem == value )
            return true;
    return false;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T> &v)
{
    os << "[ ";
    for ( const T& elem : v )
        os << elem << ' ';
    return os << ']';
}

// for use with boost
template <typename T>
std::string str(const std::vector<T> &v)
{
    std::ostringstream os;
    os << v;
    return os.str();
}

#endif
