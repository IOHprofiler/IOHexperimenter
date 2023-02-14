#pragma once
#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Sphere function problem id 1
    template<typename P=BBOB>
    class Sphere final: public P, BBOProblem<Sphere>
    {
    protected:
        //! Evaluation method
        double evaluate(const std::vector<double>& x) override
        {
            auto result = 0.0;
            for (const auto xi : x)
                result += xi * xi;
            return result;
        }
        
        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            transformation::variables::subtract(x, this->optimum_.x);
            return x;
        }
    public:
        /**
         * @brief Construct a new Sphere object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        Sphere(const int instance, const int n_variables) :
            P(1, instance, n_variables, "Sphere")
        {
        }
    };

    template class Sphere<BBOB>;
}