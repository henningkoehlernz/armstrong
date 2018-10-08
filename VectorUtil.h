#ifndef VECTOR_UTIL_H
#define VECTOR_UTIL_H
namespace vutil {

#include <bits/stdc++.h>

template <typename T>
static vector<T> operator-(const vector<T> &v, const T &value)
{
    vector<T> result;
    for ( const T &elem : v )
        if ( elem != value )
            result.push_back(elem);
    return result;
}

template <typename T>
static bool contains(const vector<T> &v, const T &value)
{
    for ( const T &elem : v )
        if ( elem == value )
            return true;
    return false;
}

template <typename T>
ostream& operator<<(ostream& os, const vector<T> &v)
{
    os << "[ ";
    for ( const T& elem : v )
        os << elem << ' ';
    return os << ']';
}

template <size_t N>
ostream& operator<<(ostream& os, const bitset<N> &bs)
{
    for ( size_t i = 0; i < N; i++ )
        os << (bs[i] ? '1' : '0');
    return os;
}

}
#endif
