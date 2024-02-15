#pragma once

#include "common.hpp"
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

    /**
     * @brief Template instantiation for real::StarDiscrepancy problems.
     *
     * @return InstanceBasedProblem::Constructors<real::StarDiscrepancy, int, int> a vector of constructor
     * functions
     */
    template <>
    inline InstanceBasedProblem::Constructors<star_discrepancy::real::StarDiscrepancy, int, int>
    InstanceBasedProblem::load_instances<star_discrepancy::real::StarDiscrepancy>(const std::optional<fs::path> &)
    {
        return star_discrepancy::load_default_instances<star_discrepancy::real::StarDiscrepancy>();
    }


} // namespace ioh::problem