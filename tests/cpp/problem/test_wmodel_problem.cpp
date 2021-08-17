#include "../utils.hpp"

#include "ioh/problem/wmodel.hpp"

TEST_F(BaseTest, WModel_Dummy)
{
    const std::vector<int> x1(16, 1);
    const std::vector<int> x0(16, 0);

    auto problem_om1 = ioh::problem::wmodel::WModelOneMax(1, 16, 0, 0, 0, 0);
    EXPECT_DOUBLE_EQ(problem_om1(x1), 16.0);
    EXPECT_DOUBLE_EQ(problem_om1(x0), 0);

    auto problem_lo1 = ioh::problem::wmodel::WModelLeadingOnes(1, 16, 0, 0, 0, 0);
    EXPECT_DOUBLE_EQ(problem_lo1(x1), 16.0);
    EXPECT_DOUBLE_EQ(problem_lo1(x0), 0);

    auto problem_om2 = ioh::problem::wmodel::WModelOneMax(1, 16, 0.5, 0, 0, 0);
    EXPECT_DOUBLE_EQ(problem_om2(x1), 8);
    EXPECT_DOUBLE_EQ(problem_om2(x0), 0);

    auto problem_lo2 = ioh::problem::wmodel::WModelLeadingOnes(1, 16, 0.5, 0, 0, 0);
    EXPECT_DOUBLE_EQ(problem_lo2(x1), 8);
    EXPECT_DOUBLE_EQ(problem_lo2(x0), 0);

    auto problem_om3 = ioh::problem::wmodel::WModelOneMax(1, 16, 0., 0, 0, 0);
    EXPECT_DOUBLE_EQ(problem_om3(x1), 16);
    EXPECT_DOUBLE_EQ(problem_om3(x0), 0);

    auto problem_lo3 = ioh::problem::wmodel::WModelLeadingOnes(1, 16, 0, 0, 0, 0);
    EXPECT_DOUBLE_EQ(problem_lo3(x1), 16);
    EXPECT_DOUBLE_EQ(problem_lo3(x0), 0);
}
