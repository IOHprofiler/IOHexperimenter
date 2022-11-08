#pragma once

#include "ioh/problem/multiobjectiveproblem.hpp"
#include "ioh/problem/problem.hpp"

namespace ioh
{
    namespace problem
    {


        class OneMinMax : public MultiObjectiveProblem<int>
        {
        protected:
            class OneMinMaxObj1 final: public PBOProblem<OneMinMaxObj1>
            {
            protected:
                //! Evaluation method
                double evaluate(const std::vector<int> &x) override { return std::accumulate(x.begin(), x.end(), 0.0); }

            public:
                OneMinMaxObj1(const int instance, const int n_variables) :
                PBOProblem(1001, instance, n_variables, "OneMinMaxObj1")
                {
                    optimum_.x = std::vector<int>(n_variables,1);
                    optimum_.y = evaluate(optimum_.x);
                    optimum_.x = reset_transform_variables(optimum_.x);
                    optimum_.y = transform_objectives(optimum_.y);
                }
            };

            class OneMinMaxObj2 final: public PBOProblem<OneMinMaxObj2>
            {
            protected:
                //! Evaluation method
                double evaluate(const std::vector<int> &x) override
                {
                    return (x.size() - std::accumulate(x.begin(), x.end(), 0.0));
                }

            public:
                OneMinMaxObj2(const int instance, const int n_variables) :
                PBOProblem(1002, instance, n_variables, "OneMinMaxObj1")
                {
                    optimum_.x = std::vector<int>(n_variables,1);
                    optimum_.y = evaluate(optimum_.x);
                    optimum_.x = reset_transform_variables(optimum_.x);
                    optimum_.y = transform_objectives(optimum_.y);
                }
            };

        public:
            /**
             * \brief Construct a new OneMax object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
             *
             * \param instance The instance number of a problem, which controls the transformation
             * performed on the original problem.
             * \param n_variables The dimensionality of the problem to created, 4 by default.
             **/
            OneMinMax(const int instance, const int n_variables, const std::string &name = "OneMaxMin") :
            MultiObjectiveProblem<int> (MetaData(1, instance, name, n_variables, common::OptimizationType::MAX, 2),
                    Bounds<int>(n_variables, 0, 1))
            {
                add_objective(std::make_unique<OneMinMaxObj1>(instance, n_variables));
                add_objective(std::make_unique<OneMinMaxObj2>(instance, n_variables));
            }
        };
    } // namespace problem
} // namespace ioh
