#pragma once

#include "ioh/common/sampler.hpp"
#include "ioh/problem/single.hpp"


namespace ioh::problem
{
    namespace star_discrepancy::real
    {
        class StarDiscrepancy : public RealSingleObjective,
                                InstanceBasedProblem,
                                AutomaticProblemRegistration<StarDiscrepancy, RealSingleObjective>,
                                AutomaticProblemRegistration<StarDiscrepancy, StarDiscrepancy>

        {
            std::vector<std::vector<double>> grid;

        protected:
            double evaluate(const std::vector<double> &x) override
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

        public:
            /**
             * @brief Construct a new StarDiscrepancy object
             *
             * @param problem_id The id of the problem
             * @param instance The instance of the problem, used as seed for the sampler
             * @param n_variables the dimension of the problem
             * @param name the name of the problem
             * @param grid the point grid
             */
            StarDiscrepancy(const int problem_id, const int instance, const int n_variables, const std::string &name,
                            const std::vector<std::vector<double>> &grid) :
                RealSingleObjective(MetaData{problem_id, instance, name, n_variables, common::OptimizationType::MAX},
                                    Bounds<double>{n_variables, 0., 1.}),
                grid(grid)
            {
                this->enforce_bounds(-std::numeric_limits<double>::infinity(), problem::constraint::Enforced::HARD);
            }

            template <typename Sampler>
            static inline std::vector<std::vector<double>> generate_grid(const int instance, const int n_variables,
                                                                         const size_t n_points)
            {
                auto sampler = Sampler(n_variables, instance);
                std::vector<std::vector<double>> res(n_points);
                for (auto &xi : res)
                    xi = sampler.next();
                return res;
            }
        };
    } // namespace star_discrepancy::real

    /**
     * @brief Template instantiation for real::StarDiscrepancy problems.
     *
     * @return InstanceBasedProblem::Constructors<real::StarDiscrepancy, int, int> a vector of contructor
     * functions
     */
    template <>
    inline InstanceBasedProblem::Constructors<star_discrepancy::real::StarDiscrepancy, int, int>
    InstanceBasedProblem::load_instances<star_discrepancy::real::StarDiscrepancy>(const std::optional<fs::path> &)
    {
        using namespace star_discrepancy::real;
        using namespace common::random::sampler;

        Constructors<StarDiscrepancy, int, int> constructors;
        int i = 0;
        std::vector<int> sample_sizes{10, 25, 50, 100, 150, 200, 250, 500, 750, 1000};

        for (const auto &sample_size : sample_sizes)
        {
            auto problem_name = fmt::format("UniformStarDiscrepancy{}", sample_size);
            constructors.push_back({[sample_size, i, problem_name](int instance, int dim) {
                                        return StarDiscrepancy(30 + i, instance, dim, problem_name,
                                                               StarDiscrepancy::generate_grid<Uniform<double>>(
                                                                   instance, dim, sample_size));
                                    },
                                    30 + i, problem_name});

            problem_name = fmt::format("SobolStarDiscrepancy{}", sample_size);
            constructors.push_back({[sample_size, i, problem_name](int instance, int dim) {
                                        return StarDiscrepancy(40 + i, instance, dim, problem_name,
                                                               StarDiscrepancy::generate_grid<Sobol>(
                                                                   instance, dim, sample_size));
                                    },
                                    40 + i, problem_name});

            problem_name = fmt::format("HaltonStarDiscrepancy{}", sample_size);
            constructors.push_back({[sample_size, i, problem_name](int instance, int dim) {
                                        return StarDiscrepancy(50 + i, instance, dim, problem_name,
                                                               StarDiscrepancy::generate_grid<Halton>(
                                                                   instance, dim, sample_size));
                                    },
                                    50 + i, problem_name});
            i++;
        }

        return constructors;
    }


} // namespace ioh::problem