#pragma once

#include "ioh/problem/functions/real.hpp"
#include "ioh/problem/single.hpp"


namespace ioh::problem
{
    namespace cec2022
    {
        /**
         * \brief The calculate_composite function is a helper function used in the computation of composite functions in optimization
         * problems. It calculates the weighted sum of several function values, which is a common approach in creating
         * hybrid functions to test optimization algorithms.
         *
         * \param x Input vector containing the coordinates in the domain space
         * \param os Transformation vectors used in the function computation.
         * \param lt Transformation matrices
         * \param deltas 
         * \param biases 
         * \param sh_rates 
         * \param s_flags 
         * \param r_flags 
         * \param f_scale 
         * \param functions the list of functions 
         * \return The weighted sum of function values.
         */
        inline double calculate_composite(
            const std::vector<double> &x, 
            const std::vector<std::vector<double>> &os,
            const std::vector<std::vector<std::vector<double>>>& lt,
            const std::vector<double> &deltas, 
            const std::vector<double> &biases,
            const std::vector<double> &sh_rates,
            const std::vector<bool>& s_flags,
            const std::vector<bool> &r_flags,
            const std::vector<double>& f_scale, 
            const std::vector<RealFunction>& functions
        )
        {
            const int nx = static_cast<int>(x.size());

            std::vector<double> fit(deltas.size());
            std::vector<double> w(fit.size(), 0.0);
            double w_max = 0.0;
            double w_sum = 0.0;

            for (size_t i = 0; i < fit.size(); i++)
            {
                std::vector<double> z(x.size());
                std::vector<double> y(x.size());
                transformation::variables::scale_and_rotate(x, z, y, os[i], lt[i], sh_rates[i], s_flags[i],
                                                            r_flags[i]);
                fit[i] = 10000 * functions[i](z) / f_scale[i];


                fit[i] += biases[i];

                for (size_t j = 0; j < x.size(); j++)
                    w[i] += std::pow(x[j] - os[i][j], 2.0);

                w[i] = abs(w[i]) > 0
                    ? std::pow(1.0 / w[i], 0.5) * std::exp(-w[i] / (2.0 * nx * std::pow(deltas[i], 2.0)))
                    : 1.0e99;

                w_max = std::max(w_max, w[i]);
                w_sum += w[i];
            }

            if (!(abs(w_max) > 0))
            {
                std::fill(w.begin(), w.end(), 1.0);
                w_sum = static_cast<double>(fit.size());
            }

            double f = 0.0;
            for (size_t i = 0; i < fit.size(); i++)
                f += (w[i] / w_sum) * fit[i];

            return f;
        }


        /**
         * \brief Calculate Hybrid function, were each of the `functions' is evaluated on
         * a portion of the search variables, defined in `fractions'.
         *
         * \param x The search variables
         * \param fractions the proportion of variables to execute each function on
         * \param scales scaling applied to each function
         * \param functions the list of functions 
         * \return the fitness 
         */
        inline double calculate_hybrid(
            const std::vector<double> &x, 
            const std::vector<double> &fractions,
            const std::vector<double> &scales,
            const std::vector<RealFunction> &functions
        )
        {
            const int nx = static_cast<int>(x.size());
            int loc = 0, i = 0;
            double fit = 0;
            while (loc < nx)
            {
                const int ng = static_cast<int>(std::ceil(fractions[i] * nx));
                const int next_loc = std::min(nx, ng + loc);
                std::vector<double> z(x.begin() + loc, x.begin() + next_loc);
                transformation::variables::scale(z, scales[i]);
                fit += functions[i++](z);
                loc += ng;
            }
            return fit;

        }
    }

    const inline std::string CEC_FOLDER = "cec_transformations/2022/";


    /// \brief The CEC2022 class represents a problem in the CEC2022 benchmark suite.
    /// It inherits from the RealSingleObjective class and contains methods to load transformation data from static files and apply transformations to problem variables and objectives.
    class CEC2022 : public RealSingleObjective
    {
        /// \brief Loads the transformation data from static files based on the problem ID and the number of variables.
        void load_transformation_data()
        {
            const int cec_function_identifier = meta_data_.problem_id - 1000;
            load_objective_shift(cec_function_identifier);
            load_linear_transformation(cec_function_identifier);
            load_variables_shift(cec_function_identifier);
            load_shuffle_data(cec_function_identifier);
            optimum_.x.assign(variables_shifts_[0].begin(),
                              variables_shifts_[0].begin() + meta_data_.n_variables);
        }

        /**
         * @brief Loads the objective shift data from a file and stores it in the objective_shift_ member variable.
         *
         */
        void load_objective_shift(const int cec_function_identifier)
        {
            const static auto shifts_file = common::file::utils::find_static_file(CEC_FOLDER + "F_i_star.txt");

            const static auto shifts = common::file::as_numeric_vector<double>(shifts_file);

            if (cec_function_identifier > 0 && cec_function_identifier <= static_cast<int>(shifts.size()))
            {
                optimum_.y = shifts[cec_function_identifier - 1];
                return;
            }

            throw std::out_of_range("CEC2022 function identifier out of range.");
        }

        /**
         * @brief Loads the linear transformation data from a file and stores it in the linear_transformations_ member
         * variable.
         *
         */
        void load_linear_transformation(const int cec_function_identifier)
        {
            const auto linear_transformation_file = common::file::utils::find_static_file(
                fmt::format("{}M_{:d}_D{:d}.txt", CEC_FOLDER, cec_function_identifier, meta_data_.n_variables));

            const auto transformation_vector = common::file::as_numeric_vector<double>(linear_transformation_file);
            const int matrix_size = meta_data_.n_variables * meta_data_.n_variables;

            for (int i = 0; i < static_cast<int>(transformation_vector.size() / matrix_size); i++)
            {
                const int start = i * matrix_size;
                linear_transformations_.push_back(common::to_matrix(std::vector<double>(
                    transformation_vector.begin() + start, transformation_vector.begin() + start + matrix_size)));
            }
        }


        /**
         * @brief Loads the variables shift data from a file and stores it in the variables_shifts_ member variable.
         *
         */
        void load_variables_shift(const int cec_function_identifier)
        {
            const auto shift_data_filepath = common::file::utils::find_static_file(
                fmt::format("{}shift_data_{}.txt", CEC_FOLDER, cec_function_identifier));

            const auto shift_data = common::file::as_numeric_vector<double>(shift_data_filepath);

            variables_shifts_ = common::to_matrix(shift_data, shift_data.size() / 100, 100);

            // This is not needed, if we take care in the transformation function
            for (auto &v : variables_shifts_)
                v.resize(meta_data_.n_variables);
        }

        /**
         * @brief Loads the shuffle data from a file and stores it in the input_permutation_ member variable.
         *
         */
        void load_shuffle_data(const int cec_function_identifier)
        {
            const auto shuffle_data_filepath = common::file::utils::get_static_root() /
                fmt::format("{}shuffle_data_{}_D{}.txt", CEC_FOLDER, cec_function_identifier, meta_data_.n_variables);

            if (exists(shuffle_data_filepath))
            {
                input_permutation_ = common::file::as_numeric_vector<int>(shuffle_data_filepath);
            }
        }

    protected:
        //! A collection of shift values applied to the problem variables during transformation.
        std::vector<std::vector<double>> variables_shifts_;

        //! A collection of matrices representing linear transformations applied to the problem variables.
        std::vector<std::vector<std::vector<double>>> linear_transformations_;

        //! A permutation vector applied to the input variables during transformation.
        std::vector<int> input_permutation_;

    public:
        /**
         * @brief Constructs a new CEC problem instance.
         *
         * @param problem_id Unique identifier for the problem.
         * @param instance The instance number of the problem (currently ignored for CEC problems).
         * @param n_variables The number of variables in the problem, representing the dimensionality of the search space.
         * @param name A unique name for the problem.
         */
        CEC2022
        (
            const int problem_id,
            const int instance,
            const int n_variables,
            const std::string &name
            ) :
            RealSingleObjective(MetaData(problem_id, instance, name, n_variables),
                                Bounds<double>(n_variables, -100, 100)),
            input_permutation_(std::vector<int>(n_variables, 0))
        {
            if (n_variables == 1)
                return;

            if (
                !(n_variables == 2 || n_variables == 10 || n_variables == 20) ||
                ((problem_id == 1006 || problem_id == 1007 || problem_id == 1008) && n_variables == 2))
            {
                std::cerr << fmt::format("[CEC2022] Problem ID: {} | Invalid n_variables: {}\n", problem_id,
                                         n_variables);
            }

            load_transformation_data();

        }

        /**
         * @brief Transforms the objective function value using the current objective shift value.
         *
         * @param y The original objective function value.
         * @return The transformed objective function value.
         */
        double transform_objectives(const double y) override
        {
            return transformation::objective::shift(y, optimum_.y);
        }

        /**
         * @brief Transforms the input variables based on the current transformation data.
         *
         * @param x The original input variables.
         * @return The transformed input variables.
         */
        std::vector<double> transform_variables(std::vector<double> x) override { return x; }
    };

    /**
     * @brief A template class for creating new CEC problems.
     * Inherit from this class when defining new CEC problems to enable automatic registration in hash maps.
     *
     * @tparam ProblemType The class representing the new CEC problem.
     */
    template <typename ProblemType>
    struct CEC2022Problem : CEC2022,
                            AutomaticProblemRegistration<ProblemType, CEC2022>,
                            AutomaticProblemRegistration<ProblemType, RealSingleObjective>
    {
        using CEC2022::CEC2022; ///< Inherits the constructor of the CEC2022 class.
    };
} // namespace ioh::problem
