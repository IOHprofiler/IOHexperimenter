#pragma once

#include "ioh/common/sampler.hpp"
#include "ioh/problem/single.hpp"


namespace ioh::problem::star_discrepancy
{
    namespace real
    {
        class StarDiscrepancy : public RealSingleObjective
        {
            std::vector<std::vector<double>> grid;
            
        protected:
            double evaluate(const std::vector<double> &x) override { 
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
             * @brief Construct a new StarDiscrepancy_ object
             *
             * @param problem_id The id of the problem
             * @param instance The instance of the problem
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
            static inline std::vector<std::vector<double>> generate_grid(const int instance, const int n_variables, const size_t n_points)
            {
                auto sampler = Sampler(n_variables, instance);
                std::vector<std::vector<double>> res(n_points);
                for (auto &xi : res)
                    xi = sampler.next();
                return res;
            }
        };


        template <typename ProblemType>
        struct StarDiscrepancy_ : StarDiscrepancy,
                                  AutomaticProblemRegistration<ProblemType, StarDiscrepancy>,
                                  AutomaticProblemRegistration<ProblemType, RealSingleObjective>
        {
            using StarDiscrepancy::StarDiscrepancy;
        };

        template <size_t N=2>
        struct UniformStarDiscrepancy : StarDiscrepancy_<UniformStarDiscrepancy<N>>
        {
            using Sampler = common::random::sampler::Uniform<double>;

            UniformStarDiscrepancy(const int instance, const int n_variables) :
                StarDiscrepancy_<UniformStarDiscrepancy<N>>(30, instance, n_variables, fmt::format("UniformStarDiscrepancy{}", N),
                                 StarDiscrepancy::generate_grid<Sampler>(instance, n_variables, N))
            {
            }
        };
        

        template <size_t N = 2>
        struct SobolStarDiscrepancy : StarDiscrepancy_<SobolStarDiscrepancy<N>>
        {
            using Sampler = common::random::sampler::Sobol;

            SobolStarDiscrepancy(const int instance, const int n_variables) :
                StarDiscrepancy_<SobolStarDiscrepancy<N>>(40, instance, n_variables, fmt::format("SobolStarDiscrepancy{}", N),
                                 StarDiscrepancy::generate_grid<Sampler>(instance, n_variables, N))
            {
            }
        };

        template <size_t N = 2>
        struct HaltonStarDiscrepancy : StarDiscrepancy_<HaltonStarDiscrepancy<N>>
        {
            using Sampler = common::random::sampler::Halton;

            HaltonStarDiscrepancy(const int instance, const int n_variables) :
                StarDiscrepancy_<HaltonStarDiscrepancy<N>>(50, instance, n_variables, fmt::format("HaltonStarDiscrepancy{}", N),
                                 StarDiscrepancy::generate_grid<Sampler>(instance, n_variables, N))
            {
            }
        };


        template <int N>
        inline int loop()
        {
            UniformStarDiscrepancy<N>(1, 1);
            SobolStarDiscrepancy<N>(1, 1);
            HaltonStarDiscrepancy<N>(1, 1);
            return loop<N * 2>();   
        }

        template <>
        inline int loop<1024>()
        {
            return 0;
        }

        static inline int __loop_invoker__ = loop<1>();


    } // namespace real

} // namespace ioh::problem::star_discrepancy