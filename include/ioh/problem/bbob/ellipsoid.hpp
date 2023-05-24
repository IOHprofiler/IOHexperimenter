#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    /**
     * @brief CTRP base class for ellipsiod class variants
     *
     * @tparam T type of the ellipsiod problem
     */
    template <typename P=BBOB>
    class EllipsoidBase : public P 
    {
    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            auto result = x.at(0) * x.at(0);
            for (auto i = 1; i < this->meta_data_.n_variables; ++i)
                result += this->transformation_state_.conditions[i] * x[i] * x[i];
            return result;
        }

        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, this->optimum_.x);
            oscillate(x);
            return x;
        }

    public:
        /**
         * @brief Construct a new Ellipsoid Base object
         *
         * @param problem_id the problem id
         * @param instance the instance
         * @param n_variables the dimension
         * @param name the name of the problem
         */
        EllipsoidBase(const int problem_id, const int instance, const int n_variables, const std::string &name) :
            P(problem_id, instance, n_variables, name)

        {
            static const auto condition = 1.0e6;
            for (auto i = 1; i < this->meta_data_.n_variables; ++i)
                this->transformation_state_.conditions[i] = pow(condition, this->transformation_state_.exponents[i]);
        }
    };

    //! Ellipsiod problem id 2
    template<typename P = BBOB>
    class Ellipsoid final : public EllipsoidBase<P>, BBOProblem<Ellipsoid>
    {
    public:
        /**
         * @brief Construct a new Ellipsoid object
         *
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        Ellipsoid(const int instance, const int n_variables) : EllipsoidBase<P>(2, instance, n_variables, "Ellipsoid") {}
    };

    template class Ellipsoid<BBOB>;
} // namespace ioh::problem::bbob
