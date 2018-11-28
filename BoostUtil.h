#ifndef BOOST_UTIL_H
#define BOOST_UTIL_H

#include <bits/stdc++.h>
#include <boost/dynamic_bitset.hpp>

namespace boost
{
    template <typename Block, typename Alloc>
    std::size_t hash_value(boost::dynamic_bitset<Block, Alloc> const& bs)
    {
        size_t seed = 0;
        std::vector<Block> blocks(bs.num_blocks());
        to_block_range(bs, blocks.begin());
        boost::hash_range(seed, blocks.begin(), blocks.end());
        return seed;
    }
}

namespace std
{
    template <typename Block, typename Alloc>
    struct hash<boost::dynamic_bitset<Block, Alloc>>
    {
        typedef boost::dynamic_bitset<Block, Alloc> argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& s) const noexcept
        {
            return hash_value(s);
        }
    };
}

#endif
