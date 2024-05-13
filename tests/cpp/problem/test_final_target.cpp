#include "../utils.hpp"
#include "ioh/problem/bbob/sphere.hpp"


TEST_F(BaseTest, test_final_target)
{
    ioh::problem::bbob::Sphere s(1, 2);

    EXPECT_EQ(s.meta_data().final_target, 1e-8);
    EXPECT_EQ(s.state().final_target_found, false);
    s(s.optimum().x);
    EXPECT_EQ(s.state().final_target_found, true);
    s.reset();
    EXPECT_EQ(s.state().final_target_found, false);

    s.set_final_target(100);
    EXPECT_EQ(s.meta_data().final_target, 100.0);
    s({0, 0});
    EXPECT_EQ(s.state().final_target_found, true);
    s.reset();
    EXPECT_EQ(s.meta_data().final_target, 100.0);
}