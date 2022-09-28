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
            MIN,
            MAX
        };

        //! Wrapper class for optimization type, contains comparison operator.
        class FOptimizationType : public common::HasRepr
        {
            //! The underlying optimization type
            OptimizationType type_;

            //! The comparison operator
            std::function<bool(double, double)> comparator_;

            //! Default (worst possible value)
            double initial_value_;

        public:
            /**
             * @brief Construct a new FOptimizationType object
             *
             * @param type The optimization type
             */
            FOptimizationType(const OptimizationType type = OptimizationType::MAX) :
                type_(type),
                comparator_(type == OptimizationType::MAX ? std::function<bool(double, double)>(std::greater<double>())
                                                          : std::function<bool(double, double)>(std::less<double>())),
                initial_value_(type == OptimizationType::MAX ? -std::numeric_limits<double>::infinity()
                                                             : std::numeric_limits<double>::infinity())
            {
            }

            //! Copy constructor
            FOptimizationType(const FOptimizationType &) = default;

            //! Accessor for type_
            OptimizationType type() const { return type_; }

            //! Accesor for initial_value_
            double initial_value() const { return initial_value_; }


            //! String representation
            std::string repr() const override
            {
                return fmt::format("<OptimizationType: {}>", type_ == OptimizationType::MAX ? "MAX" : "Minization");
            }

            /** Comparison operator between two values v1 & v2. > When OptimizationType is MAX, < otherwise.
             *
             * @return true if v1 is better than v2
             */
            bool operator()(const double v1, const double v2) const { return comparator_(v1, v2); }

            //! Equality operator
            bool operator==(const FOptimizationType &other) const { return type_ == other.type(); }

            //! Equality operator
            bool operator==(const OptimizationType &other) const { return type_ == other; }

            // //! Copy
            FOptimizationType& operator=(const FOptimizationType &) = default;
        };

    } // namespace common
} // namespace ioh


