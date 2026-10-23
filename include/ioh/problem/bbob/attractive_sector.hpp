#pragma once

#include "bbob_problem.hpp"

namespace ioh::problem::bbob
{
    //! Attractive Sector problem id = 6
    template<typename P=BBOB>
    class AttractiveSector final : public P
    {
    public:
        //! The id in the registry, also used by the constructor
        static constexpr int registry_id = 6;
        //! The name in the registry, also used by the constructor
        static constexpr const char *registry_name = "AttractiveSector";

    protected:
        //! Evaluation method
        double evaluate(const std::vector<double> &x) override
        {
            using namespace transformation::objective;
            auto result =  0.0 ;
            for (auto i = 0; i < this->meta_data_.n_variables; ++i)
                result += x[i] * x[i] * (1. + 9999.0 * (this->optimum_.x[i] * x[i] > 0.0));
            return pow(oscillate(result), .9);
        }
        
        //! Variables transformation method
        std::vector<double> transform_variables(std::vector<double> x) override
        {
            using namespace transformation::variables;
            subtract(x, this->optimum_.x);
            affine(x, this->transformation_state_.second_transformation_matrix, this->transformation_state_.transformation_base);
            return x;
        }

    public:
        /**
         * @brief Construct a new Attractive Sector object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        AttractiveSector(const int instance, const int n_variables) :
            P(registry_id, instance, n_variables, registry_name)
        {
        
        }
    };  
    extern template class AttractiveSector<BBOB>;
    extern template class AttractiveSector<SBOX>;
}
