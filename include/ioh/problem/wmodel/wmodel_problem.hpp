#pragma once

#include "ioh/problem/problem.hpp"
#include "ioh/problem/utils.hpp"

namespace ioh::problem
{
    class WModel : public Integer
    {
    protected:
        std::vector<int> dummy_info_;
        double dummy_select_rate_ = 0;
        int epistasis_block_size_ = 0;
        int neutrality_mu_ = 0;
        int ruggedness_gamma_ = 0;
        std::vector<int> ruggedness_info_;

        std::vector<int> transform_variables(std::vector<int> x) override
        {
            if (meta_data_.instance > 1 && meta_data_.instance <= 50)
                transformation::methods::transform_vars_xor(x, meta_data_.instance);
            else if (meta_data_.instance > 50 && meta_data_.instance <= 100)
                transformation::methods::transform_vars_sigma(x, meta_data_.instance);
            return x;
        }

        double transform_objectives(const double y) override
        {
            using namespace transformation::objective;
            if (meta_data_.instance > 1)
                return uniform(shift, uniform(scale, y, meta_data_.instance, -0.2, 4.8), meta_data_.instance, 1e3, 2e3);
            return y;
        }

        double evaluate(const std::vector<int> &x) override
        {
            std::vector<int> wmodel_x;

            // Dummy Layer
            if (dummy_select_rate_ > 0)
                for (const auto di : dummy_info_)
                    wmodel_x.emplace_back(x.at(di));
            else
                wmodel_x = x;

            // Neutrality layer
            if (neutrality_mu_ > 0)
                utils::layer_neutrality_compute(wmodel_x, wmodel_x, neutrality_mu_);

            // Epistasis layer
            if (epistasis_block_size_ > 0)
                utils::layer_epistasis_compute(wmodel_x, wmodel_x, epistasis_block_size_);

            auto result = wmodel_evaluate(wmodel_x);

            // Ruggedness layer
            if (ruggedness_gamma_ > 0) 
                result = ruggedness_info_.at(result);

            return static_cast<double>(result);
        }

        virtual int wmodel_evaluate(const std::vector<int> &x) = 0;

    public:
        WModel(const int problem_id, const int instance, const int n_variables, const std::string &name,
               const double dummy_select_rate, const int epistasis_block_size, const int neutrality_mu,
               const int ruggedness_gamma) :
            Integer(MetaData(problem_id, instance, name, n_variables, common::OptimizationType::Maximization)),
            dummy_select_rate_(dummy_select_rate), epistasis_block_size_(epistasis_block_size),
            neutrality_mu_(neutrality_mu), ruggedness_gamma_(ruggedness_gamma * n_variables)
        {
            auto temp_dimension = n_variables;

            if (dummy_select_rate_ > 0)
            {
                dummy_info_ = utils::dummy(temp_dimension, dummy_select_rate_, 10000);
                assert(dummy_info_.size() == static_cast<size_t>(temp_dimension * dummy_select_rate_));
                temp_dimension = static_cast<int>(dummy_info_.size());
            }

            if (neutrality_mu_ > 0)
                temp_dimension /= neutrality_mu_;

            if (ruggedness_gamma_ > 0)
                ruggedness_info_ = utils::ruggedness_raw(
                    utils::ruggedness_translate(ruggedness_gamma_, temp_dimension), temp_dimension);

            if (epistasis_block_size_ == 0)
                objective_.x = std::vector<int>(n_variables, 1);

            objective_.y = static_cast<double>(n_variables * (dummy_select_rate_ > 0 ? dummy_select_rate_ : 1) / 
                                                (neutrality_mu_ == 0 ? 1 : neutrality_mu_));
        }
    };
}
