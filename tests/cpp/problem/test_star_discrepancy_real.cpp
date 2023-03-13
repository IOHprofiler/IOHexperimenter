#include "../utils.hpp"

#include "ioh/suite.hpp"
#include "ioh/problem/star_discrepancy/real.hpp"

TEST_F(BaseTest, test_real_star_discrepancy)
{
    using namespace ioh::problem::star_discrepancy::real;
    const auto &problem_factory = ioh::problem::ProblemRegistry<StarDiscrepancy>::instance();

    EXPECT_EQ(problem_factory.ids().size(), 30);
}


TEST_F(BaseTest, test_real_star_discrepancy_uniform)
{
    using namespace ioh::problem::star_discrepancy::real;
    const auto &problem_factory = ioh::problem::ProblemRegistry<StarDiscrepancy>::instance();

    auto p2 = problem_factory.create(30, 4, 2);
        
    EXPECT_EQ((*p2)({10, 10}), -std::numeric_limits<double>::infinity());

    EXPECT_NEAR((*p2)({0.9, 0.5}), 0.25, 1e-3);
}


TEST_F(BaseTest, test_real_star_discrepancy_sobol)
{
    using namespace ioh::problem::star_discrepancy::real;
    const auto &problem_factory = ioh::problem::ProblemRegistry<StarDiscrepancy>::instance();

    auto p2 = problem_factory.create(40, 4, 2);

    EXPECT_EQ((*p2)({10, 10}), -std::numeric_limits<double>::infinity());

    EXPECT_NEAR((*p2)({0.9, 0.5}), 0.049, 1e-3);
}


TEST_F(BaseTest, test_real_star_discrepancy_halton)
{
    using namespace ioh::problem::star_discrepancy::real;
    const auto &problem_factory = ioh::problem::ProblemRegistry<StarDiscrepancy>::instance();

    auto p2 = problem_factory.create(50, 4, 2);

    EXPECT_EQ((*p2)({10, 10}), -std::numeric_limits<double>::infinity());

    EXPECT_NEAR((*p2)({0.9, 0.5}), 0.149, 1e-3);
}

TEST_F(BaseTest, test_real_star_suite){
    using namespace ioh::suite;
    StarDiscrepancy suite;
    EXPECT_EQ(suite.size(), 30);

    for (auto&problem: suite){
        const auto x0 = std::vector<double>(problem->meta_data().n_variables, 0.);
        const auto x10 = std::vector<double>(problem->meta_data().n_variables, 10.);
        EXPECT_EQ((*problem)(x10), -std::numeric_limits<double>::infinity());
        EXPECT_NE((*problem)(x0), -std::numeric_limits<double>::infinity());
        EXPECT_EQ(problem->state().evaluations, 2);
    }
}
