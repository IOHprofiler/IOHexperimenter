#pragma once

#include "ellipsoid.hpp"

namespace ioh::problem::bbob
{
    //! Rotated ellipsoid problem id 10
    template<typename P = BBOB>
    class EllipsoidRotated final : public EllipsoidBase<P>
    {
    public:
        //! The id in the registry, also used by the constructor
        static constexpr int registry_id = 10;
        //! The name in the registry, also used by the constructor
        static constexpr const char *registry_name = "EllipsoidRotated";

    protected:
        //! Transoform variables method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, this->optimum_.x);
            affine(x, this->transformation_state_.transformation_matrix,
                                                    this->transformation_state_.transformation_base);
            oscillate(x);
            return x;
        }

    public:
        /**
         * @brief Construct a new Ellipsoid Rotated object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        EllipsoidRotated(const int instance, const int n_variables) :
            EllipsoidBase<P>(registry_id, instance, n_variables, registry_name)
        {
            static const auto condition = 1.0e6;
            for (auto i = 1; i < this->meta_data_.n_variables; ++i)
                this->transformation_state_.conditions[i] = pow(condition, this->transformation_state_.exponents[i]);
        }
    };

    extern template class EllipsoidRotated<BBOB>;
    extern template class EllipsoidRotated<SBOX>;
}
