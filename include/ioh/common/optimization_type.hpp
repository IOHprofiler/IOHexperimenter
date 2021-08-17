#pragma once

namespace ioh
{
    namespace common
    {
        /**
         * \brief Enum containing minimization = 0 and maximization = 1 flags
         * 
         * @todo FIXME this should really be a class with two instances holding comparison operators, so as to avoid tests.
         */
        enum class OptimizationType
        {
            Minimization,
            Maximization
        };

        /**
         * \brief Compares two values v1 and v2, and returns true if v1 is better
         * \tparam T Type of v1 and v2
         * \param v1 The first value
         * \param v2 The second value
         * \param optimization_type Used to determine which value is better,
         * when optimization_type == \ref common::OptimizationType::Minimization lower elements are better,
         * otherwise higher elements are better.
         * \return true if v1 is better than v2
         */
        template <typename T>
        bool compare_objectives(const T v1, const T v2,
                                const OptimizationType optimization_type)
        {
            if (optimization_type == OptimizationType::Maximization)
                return v1 > v2;
            return v1 < v2;
        }
    }
}