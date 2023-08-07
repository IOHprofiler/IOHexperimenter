#pragma once

#include <Eigen/Dense>
#include <sstream>

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    class Rastrigin final : public CECProblem<Rastrigin>
    {
        int objective_shift;
        Eigen::MatrixXd linear_transformation;
        std::vector<double> variables_shift;

    protected:

        double evaluate(const std::vector<double> &x) override
        {
            auto sum1 = 0.0, sum2 = 0.0;
            for (const auto xi : x)
            {
                sum1 += cos(2.0 * IOH_PI * xi);
                sum2 += xi * xi;
            }
            if (std::isinf(sum2))
                return sum2 ;

            auto result = 10.0 * (static_cast<double>(x.size()) - sum1) + sum2;
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

            // transformed = transformed * (5.12 / 100)
            transformed_eigen *= 5.12 / 100;
            std::cout << "After scaling (* 5.12 / 100): " << transformed_eigen.transpose() << std::endl;

            if (transformed_eigen.size() > 0)
            {
                transformed_eigen = this->linear_transformation * transformed_eigen;
                std::cout << "After matrix transformation: " << transformed_eigen.transpose() << std::endl;
            }

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

        Rastrigin(const int instance, const int n_variables) :
            CECProblem
            (
                4,
                instance,
                n_variables,
                "CECRastrigin"
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
