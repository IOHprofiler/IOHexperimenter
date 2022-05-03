#pragma once

#include "ioh/common/repr.hpp"

namespace ioh
{
    /** Common utilities. */
    namespace common
    {
        //! Enum containing minimization = 0 and maximization = 1 flags
        enum class OptimizationType
        {
            Minimization,
            Maximization
        };

        //! Wrapper class for optimization type, contains comparison operator.
        class FOptimizationType : public common::HasRepr
        {
            //! The underlying optimization type
            OptimizationType type_;

            //! The comparison operator
            std::function<bool(double, double)> comparator_;

        public:
            /**
             * @brief Construct a new FOptimizationType object
             * 
             * @param type The optimization type
             */
            FOptimizationType(const OptimizationType type = OptimizationType::Maximization) :
                type_(type), comparator_(type == OptimizationType::Maximization
                                             ? std::function<bool(double, double)>(std::greater<double>())
                                             : std::function<bool(double, double)>(std::less<double>()))

            {
            }

            //! Accessor for type_
            OptimizationType type() const { return type_; }


            //! String representation 
            std::string repr() const override
            {
                return fmt::format("<OptimizationType: {}>",
                                   type_ == OptimizationType::Maximization ? "Maximization" : "Minization");
            }


            /** Comparison operator between two values v1 & v2. > When OptimizationType is Maximization, < otherwise. 
             * 
             * @return true if v1 is better than v2
             */
            bool operator()(const double v1, const double v2) const { return comparator_(v1, v2); }

            //! Equality operator
            bool operator==(FOptimizationType other) const { return type_ == other.type(); }

            //! Equality operator
            bool operator==(OptimizationType other) const { return type_ == other; }
        };

    } // namespace common
} // namespace ioh
