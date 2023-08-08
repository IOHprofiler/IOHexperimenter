#pragma once

#include <Eigen/Dense>
#include <sstream>

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class Rosenbrock final : public CECProblem<Rosenbrock>
    {
        int objective_shift;
        Eigen::MatrixXd linear_transformation;
        std::vector<double> variables_shift;

    protected:

        double evaluate(const std::vector<double> &x) override
        {
            double result = 0.0;

            std::vector<double> shifted = x; // To replace z with x and still work with the original values
            shifted[0] += 1.0; // Shift to origin

            for (size_t i = 0; i < x.size() - 1; i++)
            {
                shifted[i + 1] += 1.0; // Shift to origin
                double tmp1 = shifted[i] * shifted[i] - shifted[i + 1];
                double tmp2 = shifted[i] - 1.0;
                result += 100.0 * tmp1 * tmp1 + tmp2 * tmp2;
            }

            std::cout << "result: " << result << std::endl;

            return result;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            std::cout << "Input Vector (x): ";
            for (double val : x)
            {
                std::cout << val << " ";
            }
            std::cout << std::endl;

            // Convert x and variables_shift to Eigen vectors
            Eigen::Map<const Eigen::VectorXd> x_eigen(x.data(), x.size());
            Eigen::Map<const Eigen::VectorXd> variables_shift_eigen(this->variables_shift.data(), this->variables_shift.size());

            std::cout << "x_eigen: " << x_eigen.transpose() << std::endl;
            std::cout << "variables_shift_eigen: " << variables_shift_eigen.transpose() << std::endl;

            // transformed = x - variables_shift
            Eigen::VectorXd transformed_eigen = x_eigen - variables_shift_eigen;
            std::cout << "After subtraction (x - variables_shift): " << transformed_eigen.transpose() << std::endl;

            // transformed = transformed * (2.048 / 100)
            transformed_eigen *= 2.048 / 100;
            std::cout << "After scaling (* 2.048 / 100): " << transformed_eigen.transpose() << std::endl;

            if (transformed_eigen.size() > 0)
            {
                transformed_eigen = this->linear_transformation * transformed_eigen;
                std::cout << "After matrix transformation: " << transformed_eigen.transpose() << std::endl;
            }

            // Add 1 to every element of the Eigen vector
            transformed_eigen = transformed_eigen.array() + 1.0;

            std::cout << "After adding 1 to every element: " << transformed_eigen.transpose() << std::endl;

            // Convert the transformed Eigen vector back to std::vector
            std::vector<double> transformed(transformed_eigen.data(), transformed_eigen.data() + transformed_eigen.size());

            std::cout << "Final Transformed Vector: ";
            for (double val : transformed)
            {
                std::cout << val << " ";
            }
            std::cout << std::endl;

            return transformed;
        }

        double transform_objectives(const double y) override
        {
            return y + this->objective_shift;
        }

    public:

        Rosenbrock(const int instance, const int n_variables) :
            CECProblem
            (
                2,
                instance,
                n_variables,
                "CEC_Rosenbrock"
            )
        {
            // The only reason we allow 1 dimension is
            // because of code in line include/ioh/common/factory.hpp:207
            if (n_variables == 1) { return; }

            if (n_variables != 2 && n_variables != 10 && n_variables != 20)
            {
                std::ostringstream message;
                message << "Error: n_variables should be 2, 10, or 20. Provided n_variables: " << n_variables;
                throw std::invalid_argument(message.str());
            }

            load_transformation_data(this->objective_shift, this->linear_transformation, this->variables_shift, this->meta_data_.problem_id, n_variables);

            std::cout << "========================" << std::endl;
            std::cout << "Objective Shift: " << this->objective_shift << std::endl;
            std::cout << "Linear Transformation Matrix: \n" << this->linear_transformation << std::endl;
            std::cout << "Variables Shift: ";
            for (double value : this->variables_shift)
            {
                std::cout << value << " ";
            }
            std::cout << std::endl;
            std::cout << "========================" << std::endl;
        }
    };
}
