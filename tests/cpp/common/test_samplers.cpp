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


TEST_F(BaseTest, test_halton) { 
    ioh::common::random::sampler::Halton sampler(5, 10);
    expect_vector_eq(sampler.next(), {0.3125, 0.37037, 0.08, 0.44898, 0.909091});
    test_bounds(sampler, 0., 1.);
}