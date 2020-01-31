#ifndef BOOST_UTIL_H
#define BOOST_UTIL_H

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <boost/functional/hash.hpp>

namespace boost
{
    template <typename Block, typename Alloc>
    std::size_t hash_value(boost::dynamic_bitset<Block, Alloc> const& bs)
    {
        //static const size_t MOD = 9073277401557776713LL; // prime slightly below 2^63 with "random" bit-pattern
        size_t seed = 614889782588491517LL; // some "random" prime below MOD
        std::vector<Block> blocks(bs.num_blocks());
        to_block_range(bs, blocks.begin());
        boost::hash_range(seed, blocks.begin(), blocks.end());
        /*
        for ( int i = bs.size() - 1; i >= 0; --i )
        {
            seed = (seed << 1) % MOD;
            if ( bs[i] )
                seed++;
        }
        */
        return seed;
    }

    template <typename Block, typename Alloc>
    void reverse(boost::dynamic_bitset<Block, Alloc> & bs)
    {
        for ( size_t i = 0, swap = bs.size() - 1; i < swap; ++i, --swap )
            if ( bs[i] != bs[swap] )
            {
                bs.flip(i);
                bs.flip(swap);
            }
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
