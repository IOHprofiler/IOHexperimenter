#include "../utils.hpp"

#include "ioh/problem/star_discrepancy/star_discrepancy.hpp"

//TEST_F(BaseTest, test_real_star_discrepancy_uniform) {
//	using namespace ioh::problem::star_discrepancy::real;
//
//	UniformStarDiscrepancy<2> p(4, 2);
//
//	EXPECT_EQ(p({10, 10}), -std::numeric_limits<double>::infinity());
//
//	EXPECT_NEAR(p({0.9, 0.5}), 0.450, 1e-3);
//}
//
//
//TEST_F(BaseTest, test_real_star_discrepancy_sobol)
//{
//    using namespace ioh::problem::star_discrepancy::real;
//
//    SobolStarDiscrepancy<2> p(4, 2);
//
//    EXPECT_EQ(p({10, 10}), -std::numeric_limits<double>::infinity());
//
//    EXPECT_NEAR(p({0.9, 0.5}), 0.550, 1e-3);
//}
//
//
//TEST_F(BaseTest, test_real_star_discrepancy_halton)
//{
//    using namespace ioh::problem::star_discrepancy::real;
//    HaltonStarDiscrepancy<2> p(4, 2);
//
//    EXPECT_EQ(p({10, 10}), -std::numeric_limits<double>::infinity());
//
//    EXPECT_NEAR(p({0.9, 0.5}), 0.049, 1e-3);
//}


TEST_F(BaseTest, test_real_star_discrepancy) {
    using namespace ioh::problem::star_discrepancy::real;
    const auto &problem_factory = ioh::problem::ProblemRegistry<StarDiscrepancy>::instance();

    for (auto &[id, name] : problem_factory.map())
        std::cout << id << ", " << name << std::endl;
}