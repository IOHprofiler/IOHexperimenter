#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    /**
     * @brief CRTP base class for Schaffers Problem variants
     * 
     * @tparam T type of the new Schaffers class
     */
    template <typename T>
    class Schaffers : public BBOProblem<T>
    {
    protected:
        //! Condition of the problem
        double condition_;

        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            auto result = 0.0;
            for (size_t i = 0; i < static_cast<size_t>(this->meta_data_.n_variables) - 1; ++i)
            {
                const auto z = pow(x.at(i), 2.0) + pow(x.at(i + 1), 2.0);
                result += pow(z, 0.25) * (1.0 + pow(sin(50.0 * pow(z, 0.1)), 2.0));
            }
            return pow(result / (static_cast<double>(this->meta_data_.n_variables) - 1.0), 2.0);
        }

        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, this->optimum_.x);
            affine(x, this->transformation_state_.transformation_matrix,
                   this->transformation_state_.transformation_base);
            asymmetric(x, 0.5);
            affine(x, this->transformation_state_.second_transformation_matrix,
                   this->transformation_state_.transformation_base);
            return x;
        }

    public:
        /**
         * @brief Construct a new Schaffers object
         * 
         * @param problem_id the problem id
         * @param instance the problem instance
         * @param n_variables the problem dimension
         * @param name the name of the problem
         * @param condition condition of the problem
         */
        Schaffers(const int problem_id, const int instance, const int n_variables, const std::string &name,
                  const double condition) :
            BBOProblem<T>(problem_id, instance, n_variables, name), condition_(condition)
        {
            this->enforce_bounds(10.0);

            for (auto i = 0; i < n_variables; ++i)
                for (auto j = 0; j < n_variables; ++j)
                    this->transformation_state_.second_transformation_matrix[i][j] =
                        this->transformation_state_.second_rotation.at(i).at(j)
                        * pow(sqrt(condition), this->transformation_state_.exponents.at(i));
        }
    };

    //! Shaffers 10 problem id 17
    class Schaffers10 final : public Schaffers<Schaffers10>
    {
    public:
        /**
         * @brief Construct a new Schaffers 1 0 object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem 
         */
        Schaffers10(const int instance, const int n_variables) :
            Schaffers(17, instance, n_variables, "Schaffers10", 10.0)
        {
        }
    };
}
