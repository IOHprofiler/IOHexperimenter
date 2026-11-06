#include "../utils.hpp"

#include "ioh/suite.hpp"
#include "ioh/problem/adversarial/adversarial_attack.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <map>
#include <sstream>

namespace
{
    //! Reproduces the LCG used by amhe-attack/scripts/export_ioh_assets.py to
    //! generate the perturbation vectors behind tests/data/adversarial.in, so this
    //! test needs to store only (fid, iid, seed, y) rather than full vectors.
    std::vector<double> lcg_perturbation(uint32_t seed, size_t dim, double eps)
    {
        uint32_t s = seed & 0x7FFFFFFFu;
        std::vector<double> x(dim);
        for (size_t i = 0; i < dim; ++i)
        {
            s = (1103515245u * s + 12345u) & 0x7FFFFFFFu;
            x[i] = eps * (2.0 * static_cast<double>(s) / 2147483648.0 - 1.0);
        }
        return x;
    }
} // namespace

TEST_F(BaseTest, test_adversarial_factory_ids)
{
    const auto &factory = ioh::problem::ProblemRegistry<ioh::problem::adversarial::AdversarialAttack>::instance();
    const auto ids = factory.ids();
    EXPECT_EQ(ids.size(), 12u);
    for (const int id : {5001, 5002, 5003, 5004, 5005, 5006, 5011, 5012, 5013, 5014, 5015, 5016})
        EXPECT_NE(std::find(ids.begin(), ids.end(), id), ids.end()) << "missing preset id " << id;
}

TEST_F(BaseTest, test_adversarial_dimension_and_bounds)
{
    const auto &factory = ioh::problem::ProblemRegistry<ioh::problem::adversarial::AdversarialAttack>::instance();

    auto mnist = factory.create(5001, 1, 1); // n_variables is ignored
    EXPECT_EQ(mnist->meta_data().n_variables, 784);
    EXPECT_EQ(mnist->bounds().lb.at(0), -0.1);
    EXPECT_EQ(mnist->bounds().ub.at(0), 0.1);
    EXPECT_EQ(mnist->meta_data().optimization_type.type(), ioh::common::OptimizationType::MAX);

    auto cifar = factory.create(5011, 1, 1);
    EXPECT_EQ(cifar->meta_data().n_variables, 3072);
}

TEST_F(BaseTest, test_adversarial_shared_network)
{
    const auto &factory = ioh::problem::ProblemRegistry<ioh::problem::adversarial::AdversarialAttack>::instance();

    auto p1 = factory.create(5001, 1, 1);
    auto p2 = factory.create(5001, 2, 1);
    // Different base images (different instances)...
    EXPECT_NE(dynamic_cast<ioh::problem::adversarial::AdversarialAttack &>(*p1).true_label(),
              dynamic_cast<ioh::problem::adversarial::AdversarialAttack &>(*p2).true_label());
}

TEST_F(BaseTest, test_adversarial_regression)
{
    const auto &factory = ioh::problem::ProblemRegistry<ioh::problem::adversarial::AdversarialAttack>::instance();

    // fid -> (eps), needed to regenerate the perturbation for each row.
    const std::map<int, double> eps_of_fid = {
        {5001, 0.1}, {5002, 0.3}, {5003, 1.0}, {5004, 0.1}, {5005, 0.3}, {5006, 1.0},
        {5011, 0.1}, {5012, 0.3}, {5013, 1.0}, {5014, 0.1}, {5015, 0.3}, {5016, 1.0},
    };

    const auto path = test_data_file("adversarial.in");
    std::ifstream is(path);
    ASSERT_TRUE(is.is_open());

    int fid, iid, seed;
    double y;
    size_t n_rows = 0;
    while (is >> fid >> iid >> seed >> y)
    {
        ++n_rows;
        auto problem = factory.create(fid, iid, 1);
        const auto dim = static_cast<size_t>(problem->meta_data().n_variables);
        // Matches amhe-attack/scripts/export_ioh_assets.py: lcg_perturbation(seed=seed*1000+img_idx, ...)
        const auto composite_seed = static_cast<uint32_t>(seed) * 1000u + static_cast<uint32_t>(iid - 1);
        const auto x = lcg_perturbation(composite_seed, dim, eps_of_fid.at(fid));
        const auto got = (*problem)(x);
        // Reference values come from a from-scratch fp32 reimplementation of the CNN
        // forward pass, evaluated against the torch reference (see
        // amhe-attack/scripts/export_ioh_assets.py). Plain-loop accumulation order
        // here differs from torch's oneDNN dispatch, so the two diverge slightly more
        // as inputs grow past the model's [0, 1] training range (large eps); a mixed
        // absolute/relative tolerance avoids flaking on those extreme presets while
        // still catching real regressions (a genuine bug fails by orders of magnitude).
        const double err = std::abs(got - y);
        const double rel_err = err / std::max(1.0, std::abs(y));
        EXPECT_TRUE(err <= 1e-4 || rel_err <= 5e-3)
            << "fid=" << fid << " iid=" << iid << " seed=" << seed << " expected=" << y << " got=" << got;
    }
    EXPECT_EQ(n_rows, 180u) << "tests/data/adversarial.in row count changed unexpectedly";
}

TEST_F(BaseTest, test_adversarial_suite)
{
    using namespace ioh::suite;
    AdversarialAttack suite({5001}, {1, 2, 3}, {1});
    EXPECT_EQ(suite.size(), 3u);
    for (auto &problem : suite)
        EXPECT_EQ(problem->meta_data().n_variables, 784);
}
