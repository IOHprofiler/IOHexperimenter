#pragma once

#include "common.hpp"
#include "ioh/problem/single.hpp"

namespace ioh::problem
{
    namespace star_discrepancy::real
    {
        class StarDiscrepancy : public RealSingleObjective
        {
        public:
            //! The registry entries (see registry_entries_of): 30 problems with ids 30-59, which sample their
            //! points with the Uniform, Sobol and Halton samplers
            static std::vector<RegistryEntry<StarDiscrepancy>> registry_entries();

        private:
            Grid grid;

        protected:
            double evaluate(const std::vector<double> &x) override
            {
                return local_discrepancy(x, grid);
            }

        public:
            /**
             * @brief Construct a new (real) StarDiscrepancy object
             *
             * @param problem_id The id of the problem
             * @param instance The instance of the problem, used as seed for the sampler
             * @param n_variables the dimension of the problem
             * @param name the name of the problem
             * @param grid the point grid
             */
            StarDiscrepancy(const int problem_id, const int instance, const int n_variables, const std::string &name,
                            const Grid &grid) :
                RealSingleObjective(MetaData{problem_id, instance, name, n_variables, common::OptimizationType::MAX},
                                    Bounds<double>{n_variables, 0., 1.}),
                grid(grid)
            {
                this->enforce_bounds(-std::numeric_limits<double>::infinity(), problem::constraint::Enforced::HARD);
            }

            //! Accessor for grid
            [[nodiscard]] Grid get_grid() const { return grid; }
        };
    } // namespace star_discrepancy::real

    inline std::vector<RegistryEntry<star_discrepancy::real::StarDiscrepancy>>
    star_discrepancy::real::StarDiscrepancy::registry_entries()
    {
        return star_discrepancy::default_registry_entries<star_discrepancy::real::StarDiscrepancy>();
    }


} // namespace ioh::problem