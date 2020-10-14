/// \file f_one_max.hpp
/// \brief cpp file for class f_one_max.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#pragma once
#include "pbo_base.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            class OneMax : public pbo_base
            {
            public:
                OneMax(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
                    : pbo_base("OneMax", instance_id)
                {
                    set_best_variables(1);
                	set_number_of_variables(dimension);
                }

                double internal_evaluate(const std::vector<int>& x) {
	                auto n = x.size();
	                auto result = 0;
                    for (size_t i = 0; i != n; ++i) {
                        result += x[i];
                    }
                    return (double)result;
                }

                static OneMax* create(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
                    return new OneMax(instance_id, dimension);
                }

            };
        }
    }
}
