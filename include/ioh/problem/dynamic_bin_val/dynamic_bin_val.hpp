#pragma once

#include "ioh/problem/single.hpp"
#include "ioh/problem/transformation.hpp"

namespace ioh::problem
{
    // NOTE: It's important to give `seed` the type uint64_t instead of long;
    //   otherwise not the same pseudo-random numbers will be generated from the seed between Ubuntu and Windows.
    constexpr long max_long_value = 1782677537L;

    namespace dynamic_bin_val
    {
        struct rng
        {
            std::mt19937 gen;
            rng(const int seed) : gen(seed) {}
            long operator()() { return (gen() & max_long_value); }
        };


    } // namespace dynamic_bin_val

    class DynamicBinVal : public IntegerSingleObjective
    {

    protected:
        double evaluate(const std::vector<int> &x) override { return std::accumulate(x.begin(), x.end(), 0.0); }

        std::vector<int> transform_variables(std::vector<int> x) override
        {
            transformation::variables::random_flip(x, this->meta_data_.instance);
            return x;
        }

        double transform_objectives(const double /* y */) override
        {
            double value = 0.0;
            for (size_t i = 0; i < this->state_.current_internal.x.size(); ++i)
                value += this->state_.current_internal.x[i] * this->weights[i];
            return value;
        }

        virtual void update_weights() {}

    public:
        int time_step;
        dynamic_bin_val::rng random_generator;
        std::vector<double> weights;

        DynamicBinVal(const int problem_id, const int instance, const int n_variables, const std::string &name) :
            IntegerSingleObjective(MetaData(problem_id, instance, name, n_variables, common::OptimizationType::MAX),
                                   Bounds<int>(n_variables, 0, 1)),
            time_step(-1), random_generator(instance), weights(n_variables)
        {
            this->optimum_.x.assign(meta_data_.n_variables, 1);
            transformation::variables::random_flip(this->optimum_.x, this->meta_data_.instance);
        }

        virtual int step()
        {
            this->time_step += 1;
            update_weights();

            const auto tmp_x = this->state_.current_internal.x;
            this->state_.current_internal.x.assign(meta_data_.n_variables, 1);
            this->optimum_.y = transform_objectives(0);
            this->state_.current_internal.x = tmp_x;
            return this->time_step;
        }
    };

    template <typename ProblemType>
    struct DynamicBinValProblem : DynamicBinVal,
                                  AutomaticProblemRegistration<ProblemType, DynamicBinVal>,
                                  AutomaticProblemRegistration<ProblemType, IntegerSingleObjective>
    {
        using DynamicBinVal::DynamicBinVal;
    };
} // namespace ioh::problem
