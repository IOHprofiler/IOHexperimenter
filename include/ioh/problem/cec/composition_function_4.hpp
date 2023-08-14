#pragma once

#include "cec_problem.hpp"
#include "functions.hpp"

namespace ioh::problem::cec
{
    class CompositionFunction4 final : public CECProblem<CompositionFunction4>
    {
    protected:

        double evaluate(const std::vector<double>& x) override
        {
            // Copy the vector of vectors into ONE contiguous memory space.
            size_t total_size = 0;
            for (const auto &row : this->linear_transformation_) { total_size += row.size(); }
            // Convert into a single contiguous block
            std::vector<double> flat_data(total_size);
            size_t index = 0;
            for (const auto &row : this->linear_transformation_) { for (double val : row) { flat_data[index++] = val; } }
            double *linear_transformation_raw = &flat_data[0];
            int nx = x.size();
            double f;

            cf07(&x[0], &f, nx, &this->variables_shift_[0], linear_transformation_raw, 1);

            return f;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            return x;
        }

    public:

        CompositionFunction4(const int instance, const int n_variables) :
            CECProblem(12, instance, n_variables, "CEC_CompositionFunction4")
        {
        }
    };
}
