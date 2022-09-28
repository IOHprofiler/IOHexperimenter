#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    /**
     * @brief CRTP base class for Rosenbrock Problem variants
     * 
     * @tparam T type of the new Rosenbrock class
     */
    template<typename T>
    class RosenbrockBase : public BBOProblem<T>
    {
        double factor_;
        std::vector<double> negative_one_;
    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            auto sum1 = 0.0, sum2 = 0.0;

            for (size_t i = 0; i < x.size() - 1; ++i) {
                sum1 += pow(x.at(i) * x.at(i) - x.at(i + 1), 2.0);
                sum2 += pow(x.at(i) - 1.0, 2.0);
            }
            return 100.0 * sum1 + sum2 ;
        }
        
        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, this->optimum_.x);
            scale(x, factor_);
            subtract(x, negative_one_);
            return x;
        }

    public:
        /**
         * @brief Construct a new Rosenbrock Base object
         * 
         * @param problem_id the problem id
         * @param instance the problem instance
         * @param n_variables the problem dimension
         * @param name the name of the problem
         */
        RosenbrockBase(const int problem_id, const int instance, const int n_variables, const std::string& name) :
            BBOProblem<T>(problem_id, instance, n_variables, name),
            factor_(std::max(1.0, std::sqrt(n_variables) / 8.0)), negative_one_(n_variables, -1)
        {
        }
    };


    //! Rosenbrock problem id 8
    class Rosenbrock final: public RosenbrockBase<Rosenbrock>
    {
    public:
        /**
         * @brief Construct a new Rosenbrock object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        Rosenbrock(const int instance, const int n_variables) :
            RosenbrockBase(8, instance, n_variables, "Rosenbrock")
        {
            for (auto& e : optimum_.x)
                e *= 0.75;
        }
    };

  
}
