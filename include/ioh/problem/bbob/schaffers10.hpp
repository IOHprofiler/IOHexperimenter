#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    template <typename T>
    class Schaffers : public BBOProblem<T>
    {
    protected:
        double condition_;

        double evaluate(const std::vector<double> &x) override
        {
            auto result = 0.0;
            for (auto i = 0; i < this->meta_data_.n_variables - 1; ++i)
            {
                const auto z = pow(x.at(i), 2.0) + pow(x.at(i + 1), 2.0);
                result += pow(z, 0.25) * (1.0 + pow(sin(50.0 * pow(z, 0.1)), 2.0));
            }
            return pow(result / (static_cast<double>(this->meta_data_.n_variables) - 1.0), 2.0);
        }

        double transform_objectives(const double y) override
        {
            using namespace transformation::objective;
            static const auto penalty_factor = 10.0;
            return penalize<double>(this->state_.current.x, this->constraint_, penalty_factor,
                                    shift(y, this->objective_.y));
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_shift_evaluate_function(x, this->objective_.x);
            transform_vars_affine_evaluate_function(x, this->transformation_state_.transformation_matrix,
                                                    this->transformation_state_.transformation_base);
            transform_vars_asymmetric_evaluate_function(x, 0.5);
            transform_vars_affine_evaluate_function(x, this->transformation_state_.second_transformation_matrix,
                                                    this->transformation_state_.transformation_base);
            return x;
        }

    public:
        Schaffers(const int problem_id, const int instance, const int n_variables, const std::string &name,
                  const double condition) :
            BBOProblem<T>(problem_id, instance, n_variables, name), condition_(condition)
        {
            for (auto i = 0; i < n_variables; ++i)
                for (auto j = 0; j < n_variables; ++j)
                    this->transformation_state_.second_transformation_matrix[i][j] =
                        this->transformation_state_.second_rotation.at(i).at(j)
                        * pow(sqrt(condition), this->transformation_state_.exponents.at(i));
        }
    };

    class Schaffers10 final : public Schaffers<Schaffers10>
    {
    public:
        Schaffers10(const int instance, const int n_variables) :
            Schaffers(17, instance, n_variables, "Schaffers10", 10.0)
        {
        }
    };
}
