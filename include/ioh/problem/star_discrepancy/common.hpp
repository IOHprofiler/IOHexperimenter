#pragma once

#include "ioh/common/sampler.hpp"


namespace ioh::problem::star_discrepancy
{
    //! Typedef for point-grid types
    using Grid = std::vector<std::vector<double>>;

    /**
     * @brief Method to Generate a grid of samples
     *
     * @param instance the instance number, used as random seed for the sample
     * @param n_variables the dimension of each sample
     * @param n_points the number of samples
     * @tparam Sampler the sampler type
     * @return a grid of points
     */
    template <typename Sampler>
    [[nodiscard]] inline Grid generate_grid(const int instance, const int n_variables, const size_t n_points)
    {
        auto sampler = Sampler(n_variables, instance);
        Grid res(n_points);
        for (auto &xi : res)
            xi = sampler.next();
        return res;
    }

    /**
     * @brief local start discrepancy method for a (continous) point x
     *
     * @param x the point to be evaluated
     * @param grid the grid to compute discr on
     * @return double the local discrepancy of x using grid
     */
    [[nodiscard]] inline double local_discrepancy(const std::vector<double> &x, const Grid &grid)
    {
        double ko = 0.0, kc = 0.0, vol = 1.0;

        for (const auto &sample : grid)
        {
            double to = 1., tc = 1.;
            for (size_t i = 0; i < x.size(); i++)
            {
                if (sample[i] >= x[i])
                    to = 0.;
                if (sample[i] > x[i])
                    tc = 0.;
            }
            ko += to;
            kc += tc;
        }
        for (const auto &xi : x)
            vol *= xi;

        const double n = static_cast<double>(grid.size());
        const double disc = std::max(vol - ko / n, kc / n - vol);
        return disc;
    }

    /**
     * @brief Loader for default instances
     * 
     * @tparam T the type of StarDiscrepancy method (TODO: check for proper interface)
     * @return InstanceBasedProblem::Constructors<T, int, int> 
     */
    template <typename T>
    inline InstanceBasedProblem::Constructors<T, int, int> load_default_instances()
    {
        using namespace common::random::sampler;

        InstanceBasedProblem::Constructors<T, int, int> constructors;
        int i = 0;
        std::vector<int> sample_sizes{10, 25, 50, 100, 150, 200, 250, 500, 750, 1000};

        for (const auto &sample_size : sample_sizes)
        {
            auto problem_name = fmt::format("UniformStarDiscrepancy{}", sample_size);
            constructors.push_back(
                {[sample_size, i, problem_name](int instance, int dim) {
                     return T(30 + i, instance, dim, problem_name,
                              star_discrepancy::generate_grid<Uniform<double>>(instance, dim, sample_size));
                 },
                 30 + i, problem_name});

            problem_name = fmt::format("SobolStarDiscrepancy{}", sample_size);
            constructors.push_back({[sample_size, i, problem_name](int instance, int dim) {
                                        return T(40 + i, instance, dim, problem_name,
                                                 star_discrepancy::generate_grid<Sobol>(instance, dim, sample_size));
                                    },
                                    40 + i, problem_name}); 

            problem_name = fmt::format("HaltonStarDiscrepancy{}", sample_size);
            constructors.push_back({[sample_size, i, problem_name](int instance, int dim) {
                                        return T(50 + i, instance, dim, problem_name,
                                                 star_discrepancy::generate_grid<Halton>(instance, dim, sample_size));
                                    },
                                    50 + i, problem_name});
            i++;
        }

        return constructors;
    }


} // namespace ioh::problem::star_discrepancy
