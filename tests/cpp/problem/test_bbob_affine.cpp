#include "../utils.hpp"

#include "ioh/problem/bbob/many_affine.hpp"


TEST_F(BaseTest, TestManyAffine)
{
    using namespace ioh::problem::bbob;
    ManyAffine affine(1, 2);

    std::vector<double> x0 = {1, 2};
    EXPECT_NEAR(affine(x0), 3.521076347, 1e-8);

    //TODO: test based on indiviudal problems
}