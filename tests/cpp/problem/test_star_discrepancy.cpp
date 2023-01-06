#include "../utils.hpp"

#include "ioh/problem/star_discrepancy/star_discrepancy.hpp"

TEST_F(BaseTest, test_real_star_discrepancy)
{
    using namespace ioh::problem::star_discrepancy::real;
    const auto &problem_factory = ioh::problem::ProblemRegistry<StarDiscrepancy>::instance();

    // for (auto &[id, name] : problem_factory.map())
    //     std::cout << id << ", " << name << std::endl;

    EXPECT_EQ(problem_factory.ids().size(), 30);
}

TEST_F(BaseTest, test_real_star_discrepancy_uniform)
{
    using namespace ioh::problem::star_discrepancy::real;
    const auto &problem_factory = ioh::problem::ProblemRegistry<StarDiscrepancy>::instance();

    UniformStarDiscrepancy<2> p(4, 2);
    auto p2 = problem_factory.create(30, 4, 2);

    EXPECT_EQ(p({10, 10}), -std::numeric_limits<double>::infinity());

    EXPECT_NEAR(p({0.9, 0.5}), 0.450, 1e-3);

        
    EXPECT_EQ((*p2)({10, 10}), -std::numeric_limits<double>::infinity());

    EXPECT_NEAR((*p2)({0.9, 0.5}), 0.450, 1e-3);
}


TEST_F(BaseTest, test_real_star_discrepancy_sobol)
{
    using namespace ioh::problem::star_discrepancy::real;
    const auto &problem_factory = ioh::problem::ProblemRegistry<StarDiscrepancy>::instance();

    SobolStarDiscrepancy<2> p(4, 2);
    auto p2 = problem_factory.create(40, 4, 2);

    EXPECT_EQ(p({10, 10}), -std::numeric_limits<double>::infinity());

    EXPECT_NEAR(p({0.9, 0.5}), 0.049, 1e-3);

    EXPECT_EQ((*p2)({10, 10}), -std::numeric_limits<double>::infinity());

    EXPECT_NEAR((*p2)({0.9, 0.5}), 0.049, 1e-3);
}


TEST_F(BaseTest, test_real_star_discrepancy_halton)
{
    using namespace ioh::problem::star_discrepancy::real;
    const auto &problem_factory = ioh::problem::ProblemRegistry<StarDiscrepancy>::instance();

    HaltonStarDiscrepancy<2> p(4, 2);
    auto p2 = problem_factory.create(50, 4, 2);
    
    EXPECT_EQ(p({10, 10}), -std::numeric_limits<double>::infinity());

    EXPECT_NEAR(p({0.9, 0.5}), 0.049, 1e-3);

    EXPECT_EQ((*p2)({10, 10}), -std::numeric_limits<double>::infinity());

    EXPECT_NEAR((*p2)({0.9, 0.5}), 0.049, 1e-3);
}
