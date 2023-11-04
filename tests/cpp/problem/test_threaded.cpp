#include "../utils.hpp"

#include "ioh/problem/bbob.hpp"


TEST_F(BaseTest, TestThreaded)
{
    using namespace ioh::problem;
    using namespace ioh::problem::bbob;

    Sphere<BBOB> problem(1, 2);

    std::vector<std::vector<double>> x0 = {{1, 2}, {2, 1}};
    problem(x0);
    EXPECT_EQ(problem.state().evaluations, 2);
    EXPECT_NEAR(problem.state().current_best.y, 87.1845, 1e-3);
} 