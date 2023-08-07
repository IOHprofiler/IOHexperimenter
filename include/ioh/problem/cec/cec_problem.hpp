#pragma once

#include <Eigen/Dense>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ioh/problem/single.hpp"
#include "ioh/problem/transformation.hpp"

namespace ioh::problem
{
    class CEC : public RealSingleObjective
    {
    public:
        /**
         * @brief Construct a new CEC object
         *
         * @param problem_id The id of the problem (should be unique)
         * @param instance The instance of the problem (ignored for cec for now)
         *                 IGNORED, because the instance id is used as a random seed in BBOB problems,
         *                 but we employ no randomness in CEC,
         *                 since we take all transformation data from static files
         * @param n_variables the dimension of the problem (the size of the search space, how many x varables)
         * @param name the name of the problem (should be unique)
         * @param path the transformation file (just an example, maybe you want to do this differently)
         */
        CEC
        (
            const int problem_id,
            const int instance,
            const int n_variables,
            const std::string &name
        ) : RealSingleObjective
            (
                MetaData(problem_id, instance, name, n_variables),
                Bounds<double>(n_variables, -5, 5)
            )
        {
        }

        //! Handler for reading all static data
        void load_transformation_data
        (
            int& objective_shift,
            Eigen::MatrixXd& linear_transformation,
            std::vector<double>& variables_shift,
            const int cec_function_identifier,
            const int n_variables
        )
        {
            // Load F_i_star.
            std::ostringstream F_i_star_filepathStream;
            F_i_star_filepathStream << "include/ioh/problem/cec/input_data/F_i_star.txt";
            std::string F_i_star_filepath = F_i_star_filepathStream.str();
            load_objective_shift(objective_shift, F_i_star_filepath, cec_function_identifier);

            // Load M.
            std::ostringstream M_filepathStream;
            M_filepathStream << "include/ioh/problem/cec/input_data/M_" << cec_function_identifier << "_D" << n_variables << ".txt";
            std::string M_filepath = M_filepathStream.str();
            load_linear_transformation(linear_transformation, M_filepath);

            // Load o.
            std::ostringstream shift_data_filepathStream;
            shift_data_filepathStream << "include/ioh/problem/cec/input_data/shift_data_" << cec_function_identifier << ".txt";
            std::string shift_data_filepath = shift_data_filepathStream.str();
            load_variables_shift(variables_shift, shift_data_filepath, n_variables);
        }

        void load_objective_shift
        (
            int& objective_shift,
            const std::string& F_i_star_filepath,
            const int cec_function_identifier
        )
        {
            std::ifstream file(F_i_star_filepath); // Open the file
            if (!file.is_open()) {
                std::cerr << "Failed to open the file: " << F_i_star_filepath << std::endl;
                return;
            }

            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); // Read the file content

            std::istringstream ss(content); // Initialize the stringstream with the file content

            std::vector<int> shifts;
            int value;

            while (ss >> value) {
                shifts.push_back(value);
            }

            if (cec_function_identifier > 0 && cec_function_identifier <= shifts.size()) {
                objective_shift = shifts[cec_function_identifier - 1];
                return;
            }

            throw std::out_of_range("CEC Function identifier out of range.");
        }

        void load_linear_transformation(Eigen::MatrixXd& linear_transformation, const std::string& M_filepath)
        {
            std::ifstream file(M_filepath); // Open the file

            if (!file.is_open())
            {
                std::cerr << "Failed to open the file: " << M_filepath << std::endl;
                return;
            }

            std::vector<std::vector<double>> data;
            std::string line;

            // Read each line
            while (std::getline(file, line))
            {
                std::istringstream ss(line);
                std::vector<double> row;

                double value;
                // Tokenize by spaces and convert each token to double
                while (ss >> value)
                {
                    row.push_back(value);
                }

                data.push_back(row);
            }

            // Initialize the Eigen matrix with the data
            linear_transformation.resize(data.size(), data[0].size());
            for (size_t i = 0; i < data.size(); i++)
            {
                for (size_t j = 0; j < data[i].size(); j++)
                {
                    linear_transformation(i, j) = data[i][j];
                }
            }
        }

        void load_variables_shift
        (
            std::vector<double>& variables_shift,
            const std::string& shift_data_filepath,
            const int n_variables
        )
        {
            std::ifstream file(shift_data_filepath); // Open the file

            if (!file.is_open()) {
                std::cerr << "Failed to open the file: " << shift_data_filepath << std::endl;
                return;
            }

            std::string line;

            if (std::getline(file, line)) { // Read the entire line
                std::istringstream ss(line);
                double value;

                int count = 0;
                while (ss >> value && count < n_variables) { // Parse the tokens and convert to double
                    variables_shift.push_back(value);
                    count++;
                }
            }
        }
    };

    /**
     * @brief CRTP class for CEC problems. Inherit from this class when defining new CEC problems.
     * This is needed for storing stuff in the hash maps.
     *
     * @tparam ProblemType The New BBOB problem class
     */
    template <typename ProblemType>
    struct CECProblem : public CEC,
                        AutomaticProblemRegistration<ProblemType, CEC>,
                        AutomaticProblemRegistration<ProblemType, RealSingleObjective>
    {
    public:
        using CEC::CEC;
    };
} // namespace ioh::problem
