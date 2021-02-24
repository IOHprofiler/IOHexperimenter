#pragma once

#include "ioh/problem/problem.hpp"

namespace ioh::problem
{
    class BBOB: public RealProblem
    {
    protected:
        struct TransformationState
        {
            long seed;
            std::vector<std::vector<double>> m{};
            std::vector<double> b{};
            std::vector<std::vector<double>> rot1{};
            std::vector<std::vector<double>> rot2{};

            TransformationState(const long problem_id, const int instance, const int n_variables) :
                seed((problem_id == 4 || problem_id == 18 ? problem_id - 1 : problem_id) + 10000 * instance),
                m(n_variables, std::vector<double>(n_variables)),
                b(n_variables),
                rot1(n_variables, std::vector<double>(n_variables)),
                rot2(n_variables, std::vector<double>(n_variables))
            {
            }
        } transformation_state_;

        std::vector<double> transform_objectives(std::vector<double> y) override
        {
            transformation::coco::transform_obj_shift_evaluate_function(y, meta_data_.objective.y.at(0));
            return y;
        }
    public:
        BBOB(const int problem_id, const int instance, const int n_variables, const std::string& name) :
            RealProblem(MetaData<double>(problem_id, instance, name, n_variables, 1,
                common::OptimizationType::minimization),
                Constraint<double>(n_variables, 5, -5)),
            transformation_state_(problem_id, instance, n_variables)
        {
            meta_data_.objective = calculate_objective();
        }

        [[nodiscard]]
        Solution<double> calculate_objective() const 
        {
            using namespace transformation::coco;
            std::vector<double> x(meta_data_.n_variables, 0);
            bbob2009_compute_xopt(x, transformation_state_.seed, meta_data_.n_variables);
            return Solution<double>{
                x, { bbob2009_compute_fopt(meta_data_.problem_id, meta_data_.instance) }
            };
        }

        
    };
}
