#include "../utils.hpp"

#include "ioh/suite.hpp"
#include "ioh/problem/star_discrepancy/integer.hpp"


using namespace ioh::problem::star_discrepancy::integer;

void test_star_discrepancy(std::shared_ptr<StarDiscrepancy> p, const double expected)
{
    EXPECT_EQ((*p)({11, 11}), -std::numeric_limits<double>::infinity());
    EXPECT_NEAR((*p)({9, 9}), expected, 1e-4);
    EXPECT_NEAR((*p)({10, 10}), 0, 1e-4);
}


TEST_F(BaseTest, test_integer_star_discrepancy)
{
    const auto &problem_factory = ioh::problem::ProblemRegistry<StarDiscrepancy>::instance();

    EXPECT_EQ(problem_factory.ids().size(), 30);
}


TEST_F(BaseTest, test_integer_star_discrepancy_uniform)
{
    const auto &problem_factory = ioh::problem::ProblemRegistry<StarDiscrepancy>::instance();
    auto p2 = problem_factory.create(30, 4, 2);
    test_star_discrepancy(p2, 0.11825);
}


TEST_F(BaseTest, test_integer_star_discrepancy_sobol)
{
    const auto &problem_factory = ioh::problem::ProblemRegistry<StarDiscrepancy>::instance();

    auto p2 = problem_factory.create(40, 4, 2);
    test_star_discrepancy(p2, 0.1797);
}


TEST_F(BaseTest, test_integer_star_discrepancy_halton)
{
    const auto &problem_factory = ioh::problem::ProblemRegistry<StarDiscrepancy>::instance();

    auto p2 = problem_factory.create(50, 4, 2);
    test_star_discrepancy(p2, 0.2222);
}

TEST_F(BaseTest, test_integer_star_suite)
{
    using namespace ioh::suite;
    IntegerStarDiscrepancy suite;
    EXPECT_EQ(suite.size(), 30);

    for (auto &problem : suite)
    {
        const auto n = dynamic_cast<StarDiscrepancy *>(problem.get())->get_grid().size();
        const auto xn = std::vector<int>(problem->meta_data().n_variables, n);
        const auto xn1 = std::vector<int>(problem->meta_data().n_variables, n + 1);
        EXPECT_NEAR((*problem)(xn), 0, 1e-3);
        EXPECT_EQ((*problem)(xn1), -std::numeric_limits<double>::infinity());
        EXPECT_EQ(problem->state().evaluations, 2);
    }
}
