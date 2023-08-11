#pragma once

#include "ioh/problem/single.hpp"
#include "ioh/problem/transformation.hpp"

namespace ioh::problem
{
    class CEC : public RealSingleObjective
    {
    public:

        int objective_shift_;
        std::vector<double> variables_shift_;
        std::vector<int> input_permutation_;
        std::vector<std::vector<double>> linear_transformation_;

        /**
         * @brief Construct a new CEC object.
         *
         * @param problem_id The id of the problem (should be unique)
         * @param instance The instance of the problem (ignored for cec for now)
         *                 IGNORED, because the instance id is used as a random seed in BBOB problems,
         *                 but we employ no randomness in CEC,
         *                 since we take all transformation data from static files
         * @param n_variables the dimension of the problem (the size of the search space, how many x varables)
         * @param name the name of the problem (should be unique)
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
                Bounds<double>(n_variables, -100, 100)
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

            this->load_transformation_data();

            // Copy the from-a-static-file-loaded variables shift into the Problem.Solution.optimum_ attribute.
            this->optimum_.x = this->variables_shift_;

            std::cout << "========================" << std::endl;
            std::cout << "Objective shift: " << this->objective_shift_ << std::endl;

            std::cout << "Linear transformation matrix: " << std::endl;
            for (const auto& row : this->linear_transformation_)
            {
                for (double val : row)
                {
                    std::cout << val << " ";
                }
                std::cout << std::endl;
            }

            std::cout << "Variables shift: ";
            for (double value : this->variables_shift_)
            {
                std::cout << value << " ";
            }
            std::cout << std::endl;

            std::cout << "Input permutation: ";
            for (double value : this->input_permutation_)
            {
                std::cout << value << " ";
            }
            std::cout << std::endl;
            std::cout << "========================" << std::endl;
        }

        std::vector<double> basic_transform(std::vector<double>& x, double factor, double addend)
        {
            std::cout << "Input Vector (x): ";
            for (double val : x) {
                std::cout << val << " ";
            }
            std::cout << std::endl;

            // Subtract x and variables_shift_
            std::vector<double> transformed(x.size());
            for (size_t i = 0; i < x.size(); i++) {
                transformed[i] = x[i] - variables_shift_[i];
            }

            std::cout << "After subtraction (x - variables_shift_): ";
            for (double val : transformed) {
                std::cout << val << " ";
            }
            std::cout << std::endl;

            // Scale the transformed vector
            for (size_t i = 0; i < transformed.size(); i++) {
                transformed[i] *= factor;
            }

            std::cout << "After scaling (* " << factor << " ): ";
            for (double val : transformed) {
                std::cout << val << " ";
            }
            std::cout << std::endl;

            if (!transformed.empty()) {
                transformed = this->matVecMultiply(linear_transformation_, transformed);
                std::cout << "After matrix transformation: ";
                for (double val : transformed) {
                    std::cout << val << " ";
                }
                std::cout << std::endl;
            }

            // Add 1 to every element
            for (double& val : transformed) {
                val += addend;
            }

            std::cout << "After adding " << addend << " to every element: ";
            for (double val : transformed) {
                std::cout << val << " ";
            }
            std::cout << std::endl;

            std::cout << "Final Transformed Vector: ";
            for (double val : transformed) {
                std::cout << val << " ";
            }
            std::cout << std::endl;

            return transformed;
        }

        double transform_objectives(const double y) override
        {
            std::cout << "Objective shift: " << this->objective_shift_ << std::endl;

            auto&& transformed = y + this->objective_shift_;
            std::cout << "Transformed objective: " << transformed << std::endl;

            return transformed;
        }

        // Matrix-vector multiplication function
        std::vector<double> matVecMultiply(const std::vector<std::vector<double>>& mat, const std::vector<double>& vec)
        {
            int rows = mat.size();
            int cols = vec.size();
            std::vector<double> result(rows, 0.0);

            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    result[i] += mat[i][j] * vec[j];
                }
            }
            return result;
        }

        //! Handler for reading all static data
        void load_transformation_data()
        {
            const int n_variables = this->meta_data_.n_variables;
            const unsigned int cec_function_identifier = this->meta_data_.problem_id;

            // Load F_i_star.
            const auto F_i_star_filepath = ioh::common::file::utils::find_static_file("cec_transformations/F_i_star.txt");
            load_objective_shift(F_i_star_filepath);

            // Load M.
            std::ostringstream M_suffix_stream;
            M_suffix_stream << "cec_transformations/M_" << cec_function_identifier << "_D" << n_variables << ".txt";
            std::string M_suffix_string = M_suffix_stream.str();
            const auto M_filepath = ioh::common::file::utils::find_static_file(M_suffix_string);
            load_linear_transformation(linear_transformation_, M_filepath);

            // Load o.
            std::ostringstream shift_data_suffix_stream;
            shift_data_suffix_stream << "cec_transformations/shift_data_" << cec_function_identifier << ".txt";
            std::string shift_data_suffix_string = shift_data_suffix_stream.str();
            const auto shift_data_filepath = ioh::common::file::utils::find_static_file(shift_data_suffix_string);
            load_variables_shift(variables_shift_, shift_data_filepath);

            // Load S (only for hybrid functions).
            std::ostringstream shuffle_data_suffix_stream;
            shuffle_data_suffix_stream << "cec_transformations/shuffle_data_" << cec_function_identifier << "_D" << n_variables << ".txt";
            std::string shuffle_data_suffix_string = shuffle_data_suffix_stream.str();
            const auto shuffle_data_filepath = ioh::common::file::utils::find_static_file(shuffle_data_suffix_string);
            load_shuffle_data(shuffle_data_filepath);
        }

        void load_shuffle_data(const std::filesystem::path& shuffle_data_filepath)
        {
            const int n_variables = this->meta_data_.n_variables;

            std::ifstream file(shuffle_data_filepath); // Open the file
            if (!file.is_open())
            {
                std::cerr << "Failed to open the file: " << shuffle_data_filepath << ". Using the identity permutation on the input." << std::endl;
                this->input_permutation_.resize(n_variables);
                for(int i = 0; i < n_variables; ++i) { this->input_permutation_[i] = i + 1; }
                return;
            }

            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); // Read the file content

            std::istringstream ss(content); // Initialize the stringstream with the file content

            int value;
            this->input_permutation_ = {};
            while (ss >> value)
            {
                this->input_permutation_.push_back(value);
            }
        }

        void load_objective_shift(const std::filesystem::path& F_i_star_filepath)
        {
            const unsigned int cec_function_identifier = this->meta_data_.problem_id;

            std::ifstream file(F_i_star_filepath); // Open the file
            if (!file.is_open())
            {
                std::cerr << "Failed to open the file: " << F_i_star_filepath << std::endl;
                return;
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

        void load_linear_transformation(std::vector<std::vector<double>>& linear_transformation_, const std::filesystem::path& M_filepath)
        {
            std::ifstream file(M_filepath); // Open the file

            if (!file.is_open())
            {
                std::cerr << "Failed to open the file: " << M_filepath << std::endl;
                return;
            }

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

                linear_transformation_.push_back(row);
            }
        }

        void load_variables_shift(std::vector<double>& variables_shift_, const std::filesystem::path& shift_data_filepath)
        {
            const int n_variables = this->meta_data_.n_variables;

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
                    variables_shift_.push_back(value);
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
