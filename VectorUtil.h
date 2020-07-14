#ifndef VECTOR_UTIL_H
#define VECTOR_UTIL_H

#include <vector>
#include <sstream>
#include <unordered_set>
#include <algorithm>

namespace std
{
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

    template <typename T1, typename T2>
    std::ostream& operator<<(std::ostream& os, const std::pair<T1,T2> &v)
    {
        return os << "(" << v.first << "," << v.second << ")";
    }

    template <typename ContainerT>
    std::vector<typename ContainerT::value_type> sorted(const ContainerT &data)
    {
        std::vector<typename ContainerT::value_type> result(data.begin(), data.end());
        std::sort(result.begin(), result.end());
        return result;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& os, const std::unordered_set<T> &v)
    {
        os << "{ ";
        for ( const T& elem : std::sorted(v) )
            os << elem << ' ';
        return os << '}';
    }

    // for use with boost
    template <typename T>
    std::string str(const T &v)
    {
        std::ostringstream os;
        os << v;
        return os.str();
    }
}

#endif
