#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    template<typename T>
    class Schaffers: public BBOProblem<T>
    {
    protected:
        double condition_;
   
        std::vector<double> evaluate(std::vector<double>& x) override
        {
            auto result = 0.0;
            for (auto i = 0; i < meta_data_.n_variables - 1; ++i)
            {
                const auto z = pow(x.at(i), 2.0) + pow(x.at(i + 1), 2.0);
                result += pow(z, 0.25) * (1.0 + pow(sin(50.0 * pow(z, 0.1)), 2.0));
            }
            return { pow(result / (static_cast<double>(meta_data_.n_variables) - 1.0), 2.0) };
        }

        std::vector<double> transform_objectives(std::vector<double> y) override
        {
            using namespace transformation::coco;
            static const auto penalty_factor = 10.0;
            transform_obj_shift_evaluate_function(y, objective_.y.at(0));
            transform_obj_penalize_evaluate(state_.current.x, constraint_.lb.at(0),
                constraint_.ub.at(0), penalty_factor, y);
            return y;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::coco;
            transform_vars_shift_evaluate_function(x, objective_.x);
            transform_vars_affine_evaluate_function(x, transformation_state_.transformation_matrix,
                transformation_state_.transformation_base);
            transform_vars_asymmetric_evaluate_function(x, 0.5);
            transform_vars_affine_evaluate_function(x, transformation_state_.second_transformation_matrix,
                transformation_state_.transformation_base);
            return x;
        }
    public:
        Schaffers(const int problem_id, const int instance, const int n_variables, const std::string& name, const double condition) :
            BBOProblem(problem_id, instance, n_variables, name), condition_(condition)
        {
            for (auto i = 0; i < n_variables; ++i)
                for (auto j = 0; j < n_variables; ++j)
                    transformation_state_.second_transformation_matrix[i][j] =
                    transformation_state_.second_rotation.at(i).at(j)
                    * pow(sqrt(condition), transformation_state_.exponents.at(i));
        }
    };

    class Schaffers10 final: public Schaffers<Schaffers10>
    {
    public:
        Schaffers10(const int instance, const int n_variables) :
            Schaffers(17, instance, n_variables,  "Schaffers10", 10.0)
        {
        }
    };


}
