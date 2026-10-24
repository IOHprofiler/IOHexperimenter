#pragma once

#include "dynamic_bin_val.hpp"

namespace ioh::problem::dynamic_bin_val
{
    class Pareto final : public DynamicBinVal
    {
    public:
        //! The id in the registry, also used by the constructor
        static constexpr int registry_id = 10003;
        //! The name in the registry, also used by the constructor
        static constexpr const char *registry_name = "DynamicBinValPareto";

    private:
        double pareto_shape_;

    protected:
        void update_weights() override
        {
            // Generate uniform random doubles between 0.0 and 0.75 using the fully qualified `uniform` function
            const auto uniform_samples =
                common::random::pbo::uniform(this->weights.size(), this->random_generator(), 0.0, 0.75);

            // Transform the uniform sample to a Pareto distribution
            for (size_t i = 0; i < this->weights.size(); ++i)
                this->weights[i] = std::pow(1.0 - uniform_samples[i], -1.0 / pareto_shape_);
        }

    public:
        /**
         * @brief Constructs a new instance of DynamicBinValPowersOfTwo.
         *
         * @param instance the random instance of the problem
         * @param n_variables The dimension of the problem, representing the size of the search space and
         *                    indicating the number of variables in the problem.
         */
        Pareto(const int instance, const int n_variables) :
            DynamicBinVal(registry_id, instance, n_variables, registry_name), pareto_shape_(0.1)
        {
            step();          
        }

        [[nodiscard]] double get_pareto_shape() const { return pareto_shape_; }
    };
} // namespace ioh::problem
