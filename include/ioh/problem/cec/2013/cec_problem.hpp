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
        double transform_objectives(const double y) override
        {
            return transformation::objective::shift(-y, this->optimum_.y);
        }

        /**
         * @brief Transforms the input variables based on the current transformation data.
         *
         * @param x The original input variables.
         * @return The transformed input variables.
         */
        std::vector<double> transform_variables(std::vector<double> x) override { return x; }

        /**
         * @brief calls inner evaluate and adds a single sphere function to the location if
         * single_global_optimum is true;
         * 
         * @param x The original input variables.
         * @return the objective function value
        */
        double evaluate(const std::vector<double> &x) override {
            const double inner = (inverter_ * this->optimum_.y) - inner_evaluate(x);

            
            if (single_global_optimum)
            {
                auto x_sphere = x;
                transformation::variables::subtract(x_sphere, this->optimum_.x);
                const double flat_sphere = std::min(sphere(x_sphere), sphere_limit);

                return inner + flat_sphere;
            }                
            return inner;
        }

        //! Should be override in child classes like evaluate
        virtual double inner_evaluate(const std::vector<double> &x) = 0;

    public:
        //! Number of global optima
        size_t n_optima;

        //! The minimal distance between each optima
        double rho;

        //! Vector containing all global optima to the problem
        std::vector<Solution<double, SingleObjective>> optima;

        //! Denotes whether the optimum is enforced (a sphere function added to the location of the optimum)
        bool single_global_optimum;

        //! The maximum value that gets added to by a sphere function whenever a single global optimum is selected
        double sphere_limit = 0.01;

        /**
         * @brief Constructs a new CEC problem instance.
         *
         * @param problem_id Unique identifier for the problem.
         * @param instance The instance number of the problem (currently ignored for CEC problems).
         * @param n_variables The number of variables in the problem, representing the dimensionality of the search
         * @param lb The lower bound of the problem
         * @param ub The lower bound of the problem
         * @param opts The vector with all optima to a problem
         * @param rho The minimal distance between each optima
         * space.
         * @param name A unique name for the problem.
         */
        CEC2013(const int problem_id, const int instance, const int n_variables, const std::string &name, const double lb, const double ub, const std::vector<Solution<double, SingleObjective>>& opts, const double rho = 0.01) :
            RealSingleObjective(MetaData(problem_id, instance, name, n_variables, common::OptimizationType::MAX),
                                Bounds<double>(n_variables, lb, ub)),
            n_optima(opts.size()),
            rho(rho),
            optima(opts),
            single_global_optimum(false)
        {
            optimum_ = optima[0];
        }  

        void set_optimum(const size_t i, const bool single_global = true) 
        {
            const double y_opt = optimum_.y;
            const size_t selected_opt = i % n_optima;

            optimum_ = optima[selected_opt];
            optimum_.y = y_opt;
            single_global_optimum = single_global;

            for (size_t j = 0; j < n_optima; j++)
            {
                if(selected_opt == j) continue;
                optima[j].y = y_opt - sphere_limit;
            }
        }

        void invert() override {
            RealSingleObjective::invert();
            for(auto& sol: optima)
                sol.y *= -1;
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