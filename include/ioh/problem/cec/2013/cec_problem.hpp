#pragma once

#include "ioh/problem/functions/real.hpp"
#include "ioh/problem/single.hpp"

namespace ioh::problem
{
    class CEC2013 : public RealSingleObjective
    {
    protected:
        /**
         * @brief Transforms the objective function value using the current objective shift value.
         *
         * @param y The original objective function value.
         * @return The transformed objective function value.
         */
        double transform_objectives(const double y) override { return y; }

        /**
         * @brief Transforms the input variables based on the current transformation data.
         *
         * @param x The original input variables.
         * @return The transformed input variables.
         */
        std::vector<double> transform_variables(std::vector<double> x) override { return x; }

    public:
        //! Number of global optima
        size_t n_optima;
        /**
         * @brief Constructs a new CEC problem instance.
         *
         * @param problem_id Unique identifier for the problem.
         * @param instance The instance number of the problem (currently ignored for CEC problems).
         * @param n_variables The number of variables in the problem, representing the dimensionality of the search
         * @param lb The lower bound of the problem
         * @param ub The lower bound of the problem
         * @param n_optima The number of global optima
         * space.
         * @param name A unique name for the problem.
         */
        CEC2013(const int problem_id, const int instance, const int n_variables, const std::string &name, const double lb, const double ub, const size_t n_optima) :
            RealSingleObjective(MetaData(problem_id, instance, name, n_variables, common::OptimizationType::MAX),
                                Bounds<double>(n_variables, lb, ub)),
            n_optima(n_optima)
        {
        }  
    };

    /**
     * @brief A template class for creating new CEC problems.
     * Inherit from this class when defining new CEC problems to enable automatic registration in hash maps.
     *
     * @tparam ProblemType The class representing the new CEC problem.
     */
    template <typename ProblemType>
    struct CEC2013Problem : CEC2013,
                            InstanceBasedProblem,
                            AutomaticProblemRegistration<ProblemType, CEC2013>,
                            AutomaticProblemRegistration<ProblemType, RealSingleObjective>
    {
        using CEC2013::CEC2013; ///< Inherits the constructor of the CEC2013 class.
    };
} // namespace ioh::problem