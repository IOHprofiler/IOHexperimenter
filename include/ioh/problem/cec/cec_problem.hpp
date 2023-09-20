#pragma once

#include "ioh/problem/functions/real.hpp"
#include "ioh/problem/single.hpp"
#include "ioh/problem/transformation.hpp"

namespace ioh::problem
{
    /// \brief The CEC class represents a problem in the CEC benchmark suite.
    /// It inherits from the RealSingleObjective class and contains methods to load transformation data from static files and apply transformations to problem variables and objectives.
    class CEC : public RealSingleObjective    {
    public:

        int objective_shift_; ///< The shift value applied to the objective function to transform it.
        std::vector<std::vector<double>> variables_shifts_; ///< A collection of shift values applied to the problem variables during transformation.
        std::vector<int> input_permutation_; ///< A permutation vector applied to the input variables during transformation.
        std::vector<std::vector<std::vector<double>>> linear_transformations_; ///< A collection of matrices representing linear transformations applied to the problem variables.

        /**
         * @brief Constructs a new CEC problem instance.
         *
         * @param problem_id Unique identifier for the problem.
         * @param instance The instance number of the problem (currently ignored for CEC problems).
         * @param n_variables The number of variables in the problem, representing the dimensionality of the search space.
         * @param name A unique name for the problem.
         */
        CEC
        (
            const int problem_id,
            const int instance,
            const int n_variables,
            const std::string &name
        ) : RealSingleObjective(MetaData(problem_id, instance, name, n_variables), Bounds<double>(n_variables, -100, 100))
        {
            // Temporary work around this code: `const auto meta = T(1, 1).meta_data()`
            if (n_variables == 1) { return; }

            if (!(n_variables == 2 || n_variables == 10 || n_variables == 20))
            {
                LOG("[CEC] Problem ID: " << problem_id << " | Invalid n_variables: " << n_variables);
                std::exit(EXIT_FAILURE);
            }

            if ((problem_id == 1006 || problem_id == 1007 || problem_id == 1008) && n_variables == 2)
            {
                LOG("[CEC] Problem ID: " << problem_id << " | Invalid n_variables: " << n_variables);
                std::exit(EXIT_FAILURE);
            }

            this->load_transformation_data();

            this->optimum_.x.assign(this->variables_shifts_[0].begin(), this->variables_shifts_[0].begin() + n_variables);
        }

        /// \brief Sets the optimum value for the problem based on the current transformation data.
        void set_optimum()
        {
            const int problem_id = this->meta_data_.problem_id;
            const int n_variables = this->meta_data_.n_variables;

            // Temporary work around this code: `const auto meta = T(1, 1).meta_data()`
            if (n_variables == 1) { return; }

            if (!(n_variables == 2 || n_variables == 10 || n_variables == 20))
            {
                LOG("[set_optimum] Problem ID: " << problem_id << " | Invalid n_variables: " << n_variables);
                std::exit(EXIT_FAILURE);
            }

            this->optimum_.y = (*this)(this->optimum_.x);
        }

        /**
         * @brief Transforms the objective function value using the current objective shift value.
         *
         * @param y The original objective function value.
         * @return The transformed objective function value.
         */
        double transform_objectives(const double y) override
        {
            auto&& transformed = y + this->objective_shift_;
            return transformed;
        }

        /// \brief Loads the transformation data from static files based on the problem ID and the number of variables.
        void load_transformation_data()
        {
            const int n_variables = this->meta_data_.n_variables;
            const unsigned int cec_function_identifier = this->meta_data_.problem_id - 1000;

            // Load F_i_star.
            const std::string F_i_star_suffix_string = "cec_transformations/F_i_star.txt";
            try
            {
                const auto F_i_star_filepath = common::file::utils::find_static_file(F_i_star_suffix_string);
                load_objective_shift(F_i_star_filepath);
            }
            catch (const std::runtime_error& e)
            {
                LOG("Could not open file " << F_i_star_suffix_string << " | " << e.what());
                std::exit(EXIT_FAILURE);
            }

            // Load M.
            std::ostringstream M_suffix_stream;
            M_suffix_stream << "cec_transformations/M_" << cec_function_identifier << "_D" << n_variables << ".txt";
            std::string M_suffix_string = M_suffix_stream.str();
            try
            {
                const auto M_filepath = common::file::utils::find_static_file(M_suffix_string);
                load_linear_transformation(M_filepath);
            }
            catch (const std::runtime_error& e)
            {
                LOG("Could not open file " << M_suffix_string << " | " << e.what());
                std::exit(EXIT_FAILURE);
            }

            // Load o.
            std::ostringstream shift_data_suffix_stream;
            shift_data_suffix_stream << "cec_transformations/shift_data_" << cec_function_identifier << ".txt";
            std::string shift_data_suffix_string = shift_data_suffix_stream.str();

            try
            {
                const auto shift_data_filepath = common::file::utils::find_static_file(shift_data_suffix_string);
                load_variables_shift(shift_data_filepath);
            }
            catch (const std::runtime_error& e)
            {
                LOG("Could not open file " << shift_data_suffix_string << " | " << e.what());
                std::exit(EXIT_FAILURE);
            }

            // Load S (only for hybrid functions).
            if ((cec_function_identifier == 6 || cec_function_identifier == 7 || cec_function_identifier == 8) && (n_variables == 10 || n_variables == 20))
            {
                std::ostringstream shuffle_data_suffix_stream;
                shuffle_data_suffix_stream << "cec_transformations/shuffle_data_" << cec_function_identifier << "_D" << n_variables << ".txt";
                std::string shuffle_data_suffix_string = shuffle_data_suffix_stream.str();

                try
                {
                    const auto shuffle_data_filepath = common::file::utils::find_static_file(shuffle_data_suffix_string);
                    load_shuffle_data(shuffle_data_filepath);
                }
                catch (const std::runtime_error& e)
                {
                    LOG("Could not open file " << shuffle_data_suffix_string << " | " << e.what());
                    std::exit(EXIT_FAILURE);
                }
            }
            else
            {
                // There is no specific permutation defined for these problems.
                // Load the identity permutation.
                this->input_permutation_.resize(n_variables);
                for(int i = 0; i < n_variables; ++i) { this->input_permutation_[i] = i + 1; }
            }
        }

        /**
         * @brief Loads the shuffle data from a file and stores it in the input_permutation_ member variable.
         *
         * @param shuffle_data_filepath The path to the file containing the shuffle data.
         */
        void load_shuffle_data(const std::filesystem::path& shuffle_data_filepath)
        {
            std::ifstream file(shuffle_data_filepath); // Open the file
            if (!file.is_open())
            {
                throw std::runtime_error("Static path: \"" + shuffle_data_filepath.string() + "\"");
            }

            // Read the file content
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            std::istringstream ss(content); // Initialize the stringstream with the file content

            int value;
            this->input_permutation_ = {};
            while (ss >> value)
            {
                this->input_permutation_.push_back(value);
            }
        }

        /**
         * @brief Loads the objective shift data from a file and stores it in the objective_shift_ member variable.
         *
         * @param F_i_star_filepath The path to the file containing the objective shift data.
         */
        void load_objective_shift(const std::filesystem::path& F_i_star_filepath)
        {
            const unsigned int cec_function_identifier = this->meta_data_.problem_id - 1000;

            std::ifstream file(F_i_star_filepath); // Open the file
            if (!file.is_open())
            {
                // ASSUME: This code will never be reached,
                // because common::file::utils::find_static_file only returns existing paths.
                throw std::runtime_error("Static path: \"" + F_i_star_filepath.string() + "\"");
            }

            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); // Read the file content

            std::istringstream ss(content); // Initialize the stringstream with the file content

            std::vector<int> shifts;
            int value;

            while (ss >> value)
            {
                shifts.push_back(value);
            }

            if (cec_function_identifier > 0 && cec_function_identifier <= shifts.size())
            {
                this->objective_shift_ = shifts[cec_function_identifier - 1];
                return;
            }

            throw std::out_of_range("CEC function identifier out of range.");
        }

        /**
         * @brief Loads the linear transformation data from a file and stores it in the linear_transformations_ member variable.
         *
         * @param M_filepath The path to the file containing the linear transformation data.
         */
        void load_linear_transformation(const std::filesystem::path& M_filepath)
        {
            const size_t n_variables = this->meta_data_.n_variables;
            std::ifstream file(M_filepath); // Open the file

            if (!file.is_open())
            {
                throw std::runtime_error("Static path: \"" + M_filepath.string() + "\"");
            }

            std::string line;
            std::vector<std::vector<double>> matrix; // Temporary storage for a single matrix

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

                matrix.push_back(row);

                // If we've filled a full matrix, add it to linear_transformations_
                // and reset the temporary matrix storage
                if (matrix.size() == n_variables)
                {
                    this->linear_transformations_.push_back(matrix);
                    matrix.clear();
                }
            }

            // Check if there's a partial matrix left and handle accordingly
            if (!matrix.empty())
            {
                LOG("WARNING: Incomplete matrix detected. It will be discarded.");
                std::exit(EXIT_FAILURE);
            }
        }

        /**
         * @brief Loads the variables shift data from a file and stores it in the variables_shifts_ member variable.
         *
         * @param shift_data_filepath The path to the file containing the variables shift data.
         */
        void load_variables_shift(const std::filesystem::path& shift_data_filepath)
        {
            const int n_variables = this->meta_data_.n_variables;
            std::ifstream file(shift_data_filepath); // Open the file

            if (!file.is_open())
            {
                throw std::runtime_error("Static path: \"" + shift_data_filepath.string() + "\"");
            }

            std::string line;

            while (std::getline(file, line))
            {
                std::istringstream ss(line);
                double value;
                int count = 0; // To keep track of number of values read from the current line

                std::vector<double> current_line_values; // Temp vector to store values of the current line
                while (ss >> value && count < n_variables)
                {
                    // Parse the tokens and convert to double
                    current_line_values.push_back(value);
                    ++count;
                }

                this->variables_shifts_.push_back(current_line_values); // Add the current line values to the main vector
            }

            // Ensure that there is at least one vector inside variables_shifts_
            if (this->variables_shifts_.empty())
            {
                LOG("ERROR: No vectors were loaded into variables_shifts_");
                std::exit(EXIT_FAILURE);
            }
        }
    };

    /**
     * @brief A template class for creating new CEC problems.
     * Inherit from this class when defining new CEC problems to enable automatic registration in hash maps.
     *
     * @tparam ProblemType The class representing the new CEC problem.
     */
    template <typename ProblemType>
    struct CECProblem : public CEC,
                        AutomaticProblemRegistration<ProblemType, CEC>,
                        AutomaticProblemRegistration<ProblemType, RealSingleObjective>
    {
    public:
        using CEC::CEC; ///< Inherits the constructor of the CEC class.
    };
} // namespace ioh::problem
