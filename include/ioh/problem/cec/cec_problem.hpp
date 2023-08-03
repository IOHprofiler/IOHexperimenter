#pragma once

#include "ioh/problem/single.hpp"
#include "ioh/problem/transformation.hpp"

#include <string>
#include <vector>

namespace ioh::problem
{
    class CEC : public ioh::problem::RealSingleObjective
    {
    public:
        /**
         * @brief Construct a new CEC object
         *
         * @param problem_id The id of the problem (should be unique)
         * @param instance The instance of the problem (ignored for cec for now)
         * @param n_variables the dimension of the problem (the size of the search space, how many x varables)
         * @param name the name of the problem (should be unique)
         * @param path the transformation file (just an example, maybe you want to do this differently)
         */
        CEC(
            const int problem_id,
            const int instance,
            const int n_variables,
            const std::string &name,
            const std::string &path
        ) : ioh::problem::RealSingleObjective(
                MetaData(problem_id, instance, name, n_variables),
                Bounds<double>(n_variables, -5 /*Lower bound*/, 5 /*Upper bound*/)
            )
        {
            // this->optimum_ = set_optimum();
            // read_transformation_files(path);
        }

        //! Method to set the value for the global optimum correctly.
        void set_optimum();

        //! Handler for reading all static data
        void read_transformation_files(const std::string &path);

        //! Method for applying the cec transformations.
        std::vector<double> apply_cec_transformation(const std::vector<double> &x);

    protected:
        //! The method which should be override in ioh::problem::RealSingleObjective for applying the actual transformation correctly
        std::vector<double> transform_variables(std::vector<double> x) override {
            // return apply_cec_transformation(x);
            return x;
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
                        AutomaticProblemRegistration<ProblemType, ioh::problem::RealSingleObjective>
    {
    public:
        using CEC::CEC;
    };
} // namespace ioh::problem
