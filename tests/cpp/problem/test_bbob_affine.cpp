#include "../utils.hpp"

#include <algorithm>
#include <cstring>
#include <random>

#include "ioh/problem/bbob/many_affine.hpp"


TEST_F(BaseTest, TestManyAffine)
{
    using namespace ioh::problem::bbob;
    ManyAffine affine(1, 2);

    std::vector<double> x0 = {1, 2};
    EXPECT_NEAR(affine(x0), 3.521076347, 1e-8);

    //TODO: test based on indiviudal problems
}

namespace
{
    //! True when two doubles have the same bit pattern, which is stricter than ==.
    bool bitwise_equal(const double a, const double b) { return std::memcmp(&a, &b, sizeof(double)) == 0; }

    //! Reconstruct the original ManyAffine evaluation by summing all 24 BBOB instances.
    //! This serves as a reference for the optimized implementation.
    double evaluate_all_instances(ioh::problem::bbob::ManyAffine &affine, const std::vector<double> &x)
    {
        auto problems = affine.get_problems();
        const auto weights = affine.get_weights();
        const auto scale_factors = affine.get_scale_factors();
        const auto xopt = affine.optimum().x;

        auto result = 0.0;
        for (int fi = 0; fi < 24; fi++)
        {
            std::vector<double> x0 = x;
            for (size_t i = 0; i < x.size(); i++)
                x0[i] = x[i] + problems[fi]->optimum().x[i] - xopt[i];

            double f0 = (*problems[fi])(x0)-problems[fi]->optimum().y;
            f0 = std::min(std::max(f0, 1e-12), 1e20);
            f0 = (std::log10(f0) + 8) / scale_factors[fi];
            f0 = f0 * weights[fi];
            result += f0;
        }
        return pow(10, (10 * result - 8));
    }
} // namespace

//! Skipping zero-weight BBOB instances must not change any objective value.
//! The reference implementation evaluates all 24 BBOB instances and is compared
//! with the optimized implementation using bitwise equality.
TEST_F(BaseTest, TestManyAffineSkipsZeroWeights)
{
    using namespace ioh::problem::bbob;

    for (const int n_variables : {2, 5, 10})
    {
        for (int instance = 1; instance <= 10; instance++)
        {
            ManyAffine affine(instance, n_variables);

            const auto weights = affine.get_weights();
            const auto n_zero = std::count(weights.begin(), weights.end(), 0.0);

            // Ensure the test exercises the optimization target.
            EXPECT_GT(n_zero, 0) << "instance " << instance << " in " << n_variables << "D has no zero weight";

            // The optimum is computed inside the constructor, so it is covered too.
            EXPECT_TRUE(bitwise_equal(affine.optimum().y, evaluate_all_instances(affine, affine.optimum().x)))
                << "optimum of instance " << instance << " in " << n_variables << "D";

            std::mt19937 gen(static_cast<unsigned>(instance * 100 + n_variables));
            std::uniform_real_distribution<double> dis(-5.0, 5.0);

            std::vector<std::vector<double>> points{
                std::vector<double>(static_cast<size_t>(n_variables), 0.0),
                std::vector<double>(static_cast<size_t>(n_variables), -5.0),
                std::vector<double>(static_cast<size_t>(n_variables), 5.0),
            };
            for (int k = 0; k < 10; k++)
            {
                std::vector<double> x(static_cast<size_t>(n_variables));
                for (auto &xi : x)
                    xi = dis(gen);
                points.push_back(x);
            }

            for (const auto &x : points)
            {
                const double expected = evaluate_all_instances(affine, x);
                const double got = affine(x);
                EXPECT_TRUE(bitwise_equal(expected, got))
                    << "instance " << instance << " in " << n_variables << "D at x = " << format_vector(x)
                    << ": expected " << expected << " got " << got;
            }
        }
    }
}
