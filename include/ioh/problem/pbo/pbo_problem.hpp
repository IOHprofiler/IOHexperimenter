#pragma once

#include "ioh/problem/problem.hpp"
#include "ioh/problem/transformation.hpp"

namespace ioh::problem
{
    //! PBO base class
    class PBO : public Integer
    {
    protected:
        //! Variables transformation method
        std::vector<int> transform_variables(std::vector<int> x) override
        {
            if (meta_data_.instance > 1 && meta_data_.instance <= 50)
                transformation::variables::random_flip(x, meta_data_.instance);
            else if (meta_data_.instance > 50 && meta_data_.instance <= 100)
                transformation::variables::random_reorder(x, meta_data_.instance);
            return x;
        }
        //! Objectives transformation method
        double transform_objectives(const double y) override
        {
            using namespace transformation::objective;
            if (meta_data_.instance > 1)
                return uniform(shift, 
                            uniform(
                                scale, y, meta_data_.instance, 0.2, 5.0
                            ), 
                        meta_data_.instance, -1e3, 1e3
                    );
            return y;
        }

        //! Optimum before transformation
        std::vector<int> reset_transform_variables(std::vector<int> x)
        {
            if (meta_data_.instance > 1 && meta_data_.instance <= 50)
                transformation::variables::random_flip(x, meta_data_.instance);
            else if (meta_data_.instance > 50 && meta_data_.instance <= 100)
                x = transformation::variables::random_reorder_reset(x, meta_data_.instance);
            return x;
        }

    public:
        /**
         * @brief Construct a new PBO object
         * 
         * @param problem_id The id of the problem
         * @param instance The instance of the problem
         * @param n_variables the dimension of the problem
         * @param name the name of the problem
         */
        PBO(const int problem_id, const int instance, const int n_variables, const std::string &name) :
            Integer(MetaData(problem_id, instance, name, n_variables, common::OptimizationType::MAX),
                    Bounds<int>(n_variables, 0, 1))
        {
        }
    };

    /**
     * @brief CRTP class for PBO problems. Inherit from this class when defining new PBO problems
     * 
     * @tparam ProblemType The New PBO problem class
     */
    template <typename ProblemType>
    class PBOProblem : public PBO,
                       AutomaticProblemRegistration<ProblemType, PBO>,
                       AutomaticProblemRegistration<ProblemType, Integer>
    {
    public:
        using PBO::PBO;
    };
}
