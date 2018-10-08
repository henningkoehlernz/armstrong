#ifndef VECTOR_UTIL_H
#define VECTOR_UTIL_H

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

// for use with boost
template <typename T>
string str(const vector<T> &v)
{
    ostringstream os;
    os << v;
    return os.str();
}

#endif
