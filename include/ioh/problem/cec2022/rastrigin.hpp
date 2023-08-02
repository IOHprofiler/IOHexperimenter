#pragma once

#include "ioh/problem/bbob/bbob_problem.hpp"

namespace ioh::problem::cec2022
{
    /**
     * @brief CRTP base class for CEC2022Rastrigin Problem variants
     * 
     * @tparam T type of the new rastrigin class
     */
    template<typename P=BBOB>
    class CEC2022RastriginBase: public P
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
         * @brief Construct a new CEC2022Rastrigin Base object
         * 
         * @param problem_id the problem id
         * @param instance the problem instance
         * @param n_variables the problem dimension
         * @param name the name of the problem
         */
        CEC2022RastriginBase(const int problem_id, const int instance, const int n_variables,  const std::string& name) :
            P(problem_id, instance, n_variables, name)
        {
        }
    };

    //! CEC2022Rastrigin problem id 3
    template<typename P = BBOB>
    class CEC2022Rastrigin final: public CEC2022RastriginBase<P>, BBOProblem<CEC2022Rastrigin>
    {
    public:
        /**
         * @brief Construct a new CEC2022Rastrigin object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem 
         */
        CEC2022Rastrigin(const int instance, const int n_variables) :
            CEC2022RastriginBase<P>(3, instance, n_variables, "CEC2022Rastrigin")
        {
        }
    };
    template class CEC2022Rastrigin<BBOB>;
}
