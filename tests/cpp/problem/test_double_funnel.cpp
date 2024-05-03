#include "../utils.hpp"

#include "ioh/problem/funnel.hpp"

using namespace ioh::problem::funnel;


TEST_F(BaseTest, TestDoubleSphere)
{
    DoubleSphere instance_equal(2);
    EXPECT_EQ(instance_equal.d(), 0.0);
    EXPECT_EQ(instance_equal.s(), 1.0);
    EXPECT_EQ(instance_equal.u1(), 2.5);
    EXPECT_EQ(instance_equal.u2(), -2.5);
    EXPECT_EQ(instance_equal(instance_equal.optimum().x), instance_equal.optimum().y);
    EXPECT_EQ(instance_equal({-2.5, -2.5}), instance_equal.optimum().y);


    DoubleSphere instance_d1(2, 1);
    EXPECT_EQ(instance_d1.d(), 1.0);
    EXPECT_GT(instance_d1.u2(), -2.5);
    EXPECT_EQ(instance_d1(instance_d1.optimum().x), instance_d1.optimum().y);
    EXPECT_EQ(instance_d1({instance_d1.u2(), instance_d1.u2()}), instance_d1.optimum().y + 2.0);

    DoubleSphere instance_s0(2, 0., .2);

    std::cout << instance_s0.u2() << std::endl;

     DoubleSphere instance_s1(2, 0., 1.5);

    std::cout << instance_s1.u2() << std::endl;
}


