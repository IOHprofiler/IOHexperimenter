#include <cstdint>
#include <cstring>

#include "../utils.hpp"

#include "ioh/common/sampler.hpp"

template<typename T>
void test_bounds(ioh::common::random::sampler::Sampler<T>& sampler, T lb, T ub) {
    for (size_t i = 0; i < 10; i++)
    {
        auto p = sampler.next();
        EXPECT_EQ(p.size(), 5);
        for (const auto &xi : p)
            EXPECT_TRUE(xi >= lb && xi <= ub);
    }
}


TEST_F(BaseTest, test_unform_int)
{
    ioh::common::random::sampler::Uniform<int> sampler(5, 10, 10, 100);
    expect_vector_eq(sampler.next(), {79, 36, 11, 54, 67});
	test_bounds(sampler, 10, 100);
}


TEST_F(BaseTest, test_unform_real)
{
    ioh::common::random::sampler::Uniform<double> sampler(5, 10);
    expect_vector_eq(sampler.next(), {0.7713, 0.2987, 0.0207, 0.4945, 0.6336});
    test_bounds(sampler, 0., 1.); 
}

TEST_F(BaseTest, test_sobol) {
    
    ioh::common::random::sampler::Sobol sampler(5, 10);
    expect_vector_eq(sampler.next(), {0.9375, 0.0625, 0.5625, 0.9375, 0.3125});
    test_bounds(sampler, 0., 1.);
}

//! FNV-1a over the bit patterns of the coordinates, so that one number pins a whole sequence of points
inline std::uint64_t fnv1a(std::uint64_t hash, const double *x, const int n)
{
    for (int i = 0; i < n; i++)
    {
        std::uint64_t bits;
        std::memcpy(&bits, &x[i], sizeof bits);
        for (int byte = 0; byte < 8; byte++)
        {
            hash ^= (bits >> (8 * byte)) & 0xff;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}

TEST_F(BaseTest, test_sobol_reference_output)
{
    // Recorded before the direction numbers became a table: 64 consecutive points per dimension and
    // first seed, hashed bit for bit. A point is the XOR of the columns of V selected by the Gray code
    // of its seed; the first seeds are chosen so that every column of the table (0-12) changes some
    // hashed point (255 and 4095 add columns 7, 8, 11 and 12), and 65536 also uses columns that V
    // computes from the table. The coordinates have at most 17 significant bits, so they are exact
    // doubles and the hashes are the same on every platform. i8_sobol keeps the previous point between
    // calls, also when the dimension changes, so each dimension starts at seed 0.
    struct Case
    {
        int dim;
        long long seed;
        std::uint64_t hash;
    };
    const std::vector<Case> cases = {
        {1, 0, 0xe247677f557d98d0ULL},
        {1, 1, 0xc67053c7e025ca65ULL},
        {1, 7, 0x338dbf5b701d3a25ULL},
        {1, 255, 0x8f6825287c64e369ULL},
        {1, 1000, 0xc3d8653b985f22f3ULL},
        {1, 4095, 0xebc6ae8b83f8ac15ULL},
        {1, 65536, 0x5f8118ab52bce368ULL},
        {2, 0, 0x6c444835e41aaa05ULL},
        {2, 1, 0x16b2b0e3117a18ccULL},
        {2, 7, 0xc72f4de3f3de0c4eULL},
        {2, 255, 0xedfd6800eeff79d2ULL},
        {2, 1000, 0x03315ea877847b4cULL},
        {2, 4095, 0x7ec5135df9dd2a51ULL},
        {2, 65536, 0x5e4501c880f25132ULL},
        {5, 0, 0x46a1f06bd3544e68ULL},
        {5, 1, 0xc503fb29320b2f9eULL},
        {5, 7, 0x4d860ca2f25bc5c0ULL},
        {5, 255, 0xae490f22fa5c3861ULL},
        {5, 1000, 0x727dad6eb9c3bae9ULL},
        {5, 4095, 0xe3f2e4903e19179dULL},
        {5, 65536, 0x057c7da500d750e6ULL},
        {10, 0, 0x0e445a87e21283f5ULL},
        {10, 1, 0x4cfb72771acd04e2ULL},
        {10, 7, 0x887ed6f0c2db3790ULL},
        {10, 255, 0xdc5b327c0704ea1bULL},
        {10, 1000, 0xcf1eb494c6ba6a1bULL},
        {10, 4095, 0x4a476ed07a58d378ULL},
        {10, 65536, 0x9a23fb64cce3aeafULL},
        {40, 0, 0xd3f309dfad3757c5ULL},
        {40, 1, 0x2cb517b106463441ULL},
        {40, 7, 0xa15336f9e2c46595ULL},
        {40, 255, 0x9b83d6692245fe8cULL},
        {40, 1000, 0x249233537e8f00e2ULL},
        {40, 4095, 0x181ccb7809bf7216ULL},
        {40, 65536, 0xd307ca4aa27428a7ULL},
        {100, 0, 0xb65ba858cb893f7dULL},
        {100, 1, 0x6e8e7e85fe41e7d8ULL},
        {100, 7, 0xd27f03033fad9d8dULL},
        {100, 255, 0x196d5e46422cb54cULL},
        {100, 1000, 0x41328eb60113a2bdULL},
        {100, 4095, 0xedf9b5083e40b1faULL},
        {100, 65536, 0x95c5d32de986338bULL},
        {1111, 0, 0xdfc20b5c06618fe8ULL},
        {1111, 1, 0x872867f4d4029364ULL},
        {1111, 7, 0xac9ff7e3c16e885cULL},
        {1111, 255, 0x046ce2afb757004dULL},
        {1111, 1000, 0x6bf3ed258bb24ddcULL},
        {1111, 4095, 0x6351e8c81f9ac590ULL},
        {1111, 65536, 0xa6d375554f8c62bfULL},
    };

    std::vector<double> x(1111);
    for (const auto &c : cases)
    {
        auto seed = c.seed;
        std::uint64_t hash = 14695981039346656037ULL;
        for (int k = 0; k < 64; k++)
        {
            ioh::common::random::sampler::i8_sobol(c.dim, &seed, x.data());
            hash = fnv1a(hash, x.data(), c.dim);
        }
        EXPECT_EQ(seed, c.seed + 64);
        EXPECT_EQ(hash, c.hash) << "dimension " << c.dim << ", first seed " << c.seed;
    }
}

TEST_F(BaseTest, test_sobol_invalid_dimension)
{
    // An invalid dimension throws without disturbing the sequence of the previous dimension. The
    // valid calls use all 1111 dimensions: i8_sobol keeps its previous point when the dimension
    // changes, and restarting at seed 0 in every dimension leaves a consistent point for later tests.
    // The first coordinate is the same in every dimension.
    std::vector<double> x(1112);
    long long seed = 5;
    ioh::common::random::sampler::i8_sobol(1111, &seed, x.data());
    EXPECT_THROW(ioh::common::random::sampler::i8_sobol(1112, &seed, x.data()), std::invalid_argument);

    seed = 0;
    for (const double expected : {0.0, 0.5, 0.75, 0.25, 0.375, 0.875, 0.625, 0.125})
    {
        ioh::common::random::sampler::i8_sobol(1111, &seed, x.data());
        EXPECT_EQ(x[0], expected) << "seed " << seed - 1;
    }
}


TEST_F(BaseTest, test_halton) { 
    ioh::common::random::sampler::Halton sampler(5, 10);
    expect_vector_eq(sampler.next(), {0.3125, 0.37037, 0.08, 0.44898, 0.909091});
    test_bounds(sampler, 0., 1.);
}