#pragma once

#include "bbob_problem.hpp" 

namespace ioh::problem::bbob
{
    namespace many_affine {
        inline std::array<double, 24> get_weights(const int instance){
            auto weights = common::random::bbob2009::uniform(24, 2000 + instance, 0., 1.);
            
            // Make the two largest weights larger than 0.85.
            // There shall be at least two functions being combined.
            size_t max1_i = 0, max2_i = 0;
            double max1 = -1;
            double max2 = -2;
            for (size_t i = 0; i != weights.size(); ++i) {
                if(weights[i] > max1) {
                    max2 = max1;
                    max2_i = max1_i;

                    max1 = weights[i];
                    max1_i = i;
                } else if (weights[i] > max2) {
                    max2 = weights[i];
                    max2_i = i;
                }
            }
            weights[max1_i] = weights[max1_i] > 0.85 ? weights[max1_i] : 0.85;
            weights[max2_i] = weights[max2_i] > 0.85 ? weights[max2_i] : 0.85;

            auto sum = 0.;
            for (auto &wi: weights){
                if (wi >= 0.85){
                    sum += wi;
                } else {
                    wi = 0;
                }
            }
            for (auto &wi: weights){
                wi /= sum;
            }
            return common::from_vector<double, 24>(weights);
        }
    }
    
    class ManyAffine final : public RealSingleObjective
    {
    public:
        static inline constexpr std::array<double, 24> default_scales{11.,  17.5, 12.3, 12.6, 11.5, 15.3, 12.1, 15.3,
                                                                      15.2, 17.4, 13.4, 20.4, 12.9, 10.4, 12.3, 10.3,
                                                                      9.8,  10.6, 10.,  14.7, 10.7, 10.8, 9.,   12.1};

        /**
         * @brief Construct a new Many Affine object, requires weights and instances to be specified
         * @param xopt the location of the optimum
         * @param weights weight vector which specifies the contribution of each of the 24 bbob to the final
         * objective value
         * @param instances the vector of instances used
         * @param n_variables the dimension of the problem
         * @param scale_factors the relative scale factors for each problem
         */
        ManyAffine(const std::vector<double> &xopt, const std::array<double, 24> &weights,
                   const std::array<int, 24> &instances, const int n_variables,
                   const std::array<double, 24> &scale_factors) :
            RealSingleObjective(MetaData(0, instances[0], "ManyAffine", n_variables),
                                Bounds<double>(n_variables, -5, 5)),
            weights_(weights), instances_(instances), scale_factors_(scale_factors), problems_{}, function_values_{}
        {
            const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::BBOB>::instance();
            for (int fid = 1; fid < 25; fid++)
                problems_[fid - 1] = problem_factory.create(fid, instances[fid - 1], n_variables);

            this->optimum_.x = xopt;
            this->optimum_.y = evaluate(xopt);
        }

        /**
         * @brief Construct a new Many Affine object with default weights and a single instance for all sub functions.
         * A random location of the optimum and a weight vector are generated using the bbob2009 uniform random number
         * generator, using the instance as seed offset.
         *
         * @param instance the instance
         * @param n_variables the dimension used
         */
        ManyAffine(const int instance, const int n_variables) :
            ManyAffine(common::random::bbob2009::uniform(n_variables, 1000 + instance, -4., 4.),
                       many_affine::get_weights(instance),
                       common::fill_array<int, 24>(instance), n_variables, ManyAffine::default_scales)
        {
        }

    protected:
        double evaluate(const std::vector<double> &x) override
        {
            auto result = 0.0;
            for (int fi = 0; fi < 24; fi++)
            {
                // compute xopt shifted x
                std::vector<double> x0 = x;
                for (size_t i = 0; i < x.size(); i++)
                {
                    x0[i] = x[i] + problems_[fi]->optimum().x[i] - this->optimum_.x[i];
                }
                // evaluate and shift by yopt
                double f0 = (*problems_[fi])(x0)-problems_[fi]->optimum().y;
                // clamp to finite
                f0 = std::min(std::max(f0, 1e-12), 1e20);
                // apply scaling
                f0 = (std::log10(f0) + 8) / scale_factors_[fi];
                // apply weights
                f0 = f0 * weights_[fi];
                result += f0;
        }
            // convert to base 10
            result = pow(10, (10 * result - 8));
            return result;
        }

    private:
        std::array<double, 24> weights_;
        std::array<int, 24> instances_;
        std::array<double, 24> scale_factors_;
        std::array<std::shared_ptr<ioh::problem::BBOB>, 24> problems_;
        std::array<double, 24> function_values_;

    public:
        [[nodiscard]] std::array<double, 24> get_weights() const
        {
            return weights_;
        }

        [[nodiscard]] std::array<int, 24> get_instances() const
        {
            return instances_;
        }

        [[nodiscard]] std::array<double, 24> get_scale_factors() const
        {
            return scale_factors_;
        }
        std::array<std::shared_ptr<ioh::problem::BBOB>, 24> get_problems(){
            return problems_;
        }

        [[nodiscard]] std::array<double, 24> get_function_values() const
        {
            return function_values_;
        }
    };
} // namespace ioh::problem::bbob
