#pragma once

#include "cec_problem.hpp"
#include "functions.hpp"

namespace ioh::problem::cec
{
    class CompositionFunction3 final : public CECProblem<CompositionFunction3>
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

            double f;
            cf06(x, f, this->variables_shift_, flat_data, 1);

            return f;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            return x;
        }

    public:

        CompositionFunction3(const int instance, const int n_variables) :
            CECProblem(11, instance, n_variables, "CEC_CompositionFunction3")
        {
        }
    };
}
