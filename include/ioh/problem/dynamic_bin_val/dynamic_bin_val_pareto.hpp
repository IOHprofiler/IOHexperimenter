#pragma once

#include "dynamic_bin_val.hpp"

namespace ioh::problem
{
    class DynamicBinValPareto final : public DynamicBinValProblem<DynamicBinValPareto>
    {
    public:

        double pareto_shape;
        int timestep;
        std::default_random_engine random_generator;
        std::vector<double> weights;
        std::vector<int> transformed_x;

        DynamicBinValPareto(const int instance, const int n_variables) :
            DynamicBinValProblem(
                10'003,
                instance,
                n_variables,
                "DynamicBinValPareto"
            ),
            pareto_shape(0.1),
            timestep(0),
            random_generator(instance)
        {
            if (n_variables == 1) { return; }

            this->weights.resize(n_variables);

            std::uniform_real_distribution<double> distribution(0.0, 0.75);

            for(size_t i = 0; i < this->weights.size(); ++i)
            {
                double uniform_sample = distribution(this->random_generator);

                // Calculate the weight using the power-law distribution inversion formula
                // Truncate the distribution to prevent overflow when weights are summed
                auto pareto_distributed = std::pow(1.0 - uniform_sample, -1.0 / pareto_shape);
                this->weights[i] = static_cast<double>(pareto_distributed);
            }

            this->transformed_x = std::vector<int>(this->weights.size(), 1);
            this->optimum_.y = transform_objectives(0);
            transform_variables(this->transformed_x);
            this->optimum_.x = this->transformed_x;
        }

        int step() override
        {
            this->timestep += 1;

            std::uniform_real_distribution<double> distribution(0.0, 0.75);

            for(size_t i = 0; i < this->weights.size(); ++i)
            {
                double uniform_sample = distribution(this->random_generator);
                auto pareto_distributed = std::pow(1.0 - uniform_sample, -1.0 / pareto_shape);
                this->weights[i] = static_cast<double>(pareto_distributed);
            }

            this->transformed_x = std::vector<int>(this->weights.size(), 1);
            this->optimum_.y = transform_objectives(0);
            transform_variables(this->transformed_x);
            this->optimum_.x = this->transformed_x;

            return this->timestep;
        }

        double get_pareto_shape() const
        {
            return pareto_shape;
        }

    protected:

        double evaluate(const std::vector<int> &x) override
        {
            return std::accumulate(x.begin(), x.end(), 0.0);
        }

        std::vector<int> transform_variables(std::vector<int> x) override
        {
            transformation::variables::random_flip(x, this->meta_data_.instance);
            this->transformed_x = x;
            return x;
        }

        double transform_objectives(const double /* y */) override
        {
            double value = 0.0;
            for(size_t i = 0; i < this->transformed_x.size(); ++i)
            {
                value += this->transformed_x[i] * this->weights[i];
            }
            return value;
        }
    };
}
