#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    /**
     * @brief CRTP base class for Rastrigin Problem variants
     * 
     * @tparam T type of the new rastrigin class
     */
    template<typename T>
    class RastriginBase: public BBOProblem<T>
    {
    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            auto sum1 = 0.0, sum2 = 0.0;

            for (const auto xi : x)
            {
                sum1 += cos(2.0 * IOH_PI * xi);
                sum2 += xi * xi;
            }
            if (std::isinf(sum2))
                return sum2 ;

            return 10.0 * (static_cast<double>(x.size()) - sum1) + sum2;
        }

        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, this->optimum_.x);
            oscillate(x);
            asymmetric(x, 0.2);
            conditioning(x, 10.0);
            return x;
        }

    public:
        /**
         * @brief Construct a new Rastrigin Base object
         * 
         * @param problem_id the problem id
         * @param instance the problem instance
         * @param n_variables the problem dimension
         * @param name the name of the problem
         */
        RastriginBase(const int problem_id, const int instance, const int n_variables,  const std::string& name ) :
            BBOProblem<T>(problem_id, instance, n_variables, name)
        {
        }
    };

    //! Rastrigin problem id 3
    class Rastrigin final: public RastriginBase<Rastrigin>
    {
    public:
        /**
         * @brief Construct a new Rastrigin object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem 
         */
        Rastrigin(const int instance, const int n_variables) :
            RastriginBase(3, instance, n_variables, "Rastrigin")
        {
        }
    };


}
