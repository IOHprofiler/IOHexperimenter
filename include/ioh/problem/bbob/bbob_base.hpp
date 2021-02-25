#pragma once

#include "ioh/problem/problem.hpp"
#include "ioh/problem/registry.hpp"

namespace ioh::problem
{
    class BBOBProblem : public RealProblem
    {
    protected:
        struct TransformationState
        {
            long seed;
            std::vector<double> raw_x{};
            std::vector<double> exponents{};
            std::vector<double> conditions{};
            std::vector<std::vector<double>> m{};
            std::vector<double> b{};
            std::vector<std::vector<double>> rot1{};
            std::vector<std::vector<double>> rot2{};

            TransformationState(const long problem_id, const int instance, const int n_variables) :
                seed((problem_id == 4 || problem_id == 18 ? problem_id - 1 : problem_id) + 10000 * instance),
                raw_x(n_variables),
                exponents(n_variables),
                conditions(n_variables),
                m(n_variables, std::vector<double>(n_variables)),
                b(n_variables),
                rot1(n_variables, std::vector<double>(n_variables)),
                rot2(n_variables, std::vector<double>(n_variables))
            {
                using namespace transformation::coco;

                for (auto i = 0; i < n_variables; ++i) 
                    exponents[i] = static_cast<double>(i) / (static_cast<double>(n_variables) - 1);

                bbob2009_compute_rotation(rot1, seed + 1000000, n_variables);
                bbob2009_compute_rotation(rot2, seed, n_variables);
            }
        } transformation_state_;

        // void reset() override
        // {
        //     RealProblem::reset();
        //     transformation_state_ = {
        //         meta_data_.problem_id, meta_data_.instance, meta_data_.n_variables
        //     };
        // }

        virtual std::vector<double> transform_objectives(std::vector<double> y) override
        {
            transformation::coco::transform_obj_shift_evaluate_function(y, meta_data_.objective.y.at(0));
            return y;
        }

    public:
        BBOBProblem(const int problem_id, const int instance, const int n_variables, const std::string &name):
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
                x, {bbob2009_compute_fopt(meta_data_.problem_id, meta_data_.instance)}
            };
        }
    };

    template <typename ProblemType>
    class BBOB : public BBOBProblem,
                 AutomaticFactoryRegistration<ProblemType, BBOBProblem>,
                 AutomaticFactoryRegistration<ProblemType, RealProblem>
    {
    public:
        using BBOBProblem::BBOBProblem;
    };
}
