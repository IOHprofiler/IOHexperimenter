#pragma once

#include "common.hpp"
#include "ioh/problem/single.hpp"

namespace ioh::problem
{
    namespace star_discrepancy::integer
    {
        class StarDiscrepancy : public IntegerSingleObjective
        {
        public:
            //! The registry entries (see registry_entries_of): 30 problems with ids 30-59, which sample their
            //! points with the Uniform, Sobol and Halton samplers
            static std::vector<RegistryEntry<StarDiscrepancy>> registry_entries();

        private:
            //! The sampled points
            Grid grid;
            //! Same a grid, but with last element a vector of all ones
            Grid internal_grid;
            //! Preallocated double representation of the added sample
            std::vector<double> dx;
            
        protected:
            double evaluate(const std::vector<int> &x) override
            {
                for(size_t i = 0; i < x.size(); i++)
                    dx[i] = internal_grid[x[i]][i];

                return local_discrepancy(dx, grid);
            }

        public:
            /**
             * @brief Construct a new (integer) StarDiscrepancy object
             *
             * @param problem_id The id of the problem
             * @param instance The instance of the problem, used as seed for the sampler
             * @param n_variables the dimension of the problem
             * @param name the name of the problem
             * @param grid the point grid
             */
            StarDiscrepancy(const int problem_id, const int instance, const int n_variables, const std::string &name,
                            const Grid &grid) :
                IntegerSingleObjective(MetaData{problem_id, instance, name, n_variables, common::OptimizationType::MAX},
                                       Bounds<int>{n_variables, 0, static_cast<int>(grid.size())}),
                grid(grid), internal_grid(ioh::common::sort_colwise(grid)), dx(n_variables)
            {
                this->enforce_bounds(-std::numeric_limits<double>::infinity(), problem::constraint::Enforced::HARD);
                this->internal_grid.emplace_back(n_variables, 1.);
            }

            //! Accessor for grid
            [[nodiscard]] Grid get_grid() const { return grid; }
        };
    } // namespace star_discrepancy::integer

    inline std::vector<RegistryEntry<star_discrepancy::integer::StarDiscrepancy>>
    star_discrepancy::integer::StarDiscrepancy::registry_entries()
    {
        return star_discrepancy::default_registry_entries<star_discrepancy::integer::StarDiscrepancy>();
    }

} // namespace ioh::problem
