/// \file f_one_max_epistasis.hpp
/// \brief cpp file for class f_one_max_epistasis.
///
/// This file implements a OneMax problem with epistasis transformation method from w-model.
/// The parameter v is chosen as 4.
///
/// \author Furong Ye
/// \date 2019-06-27
#pragma once
#include "pbo_base.hpp"
#include "ioh/problem/utils.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            class OneMax_Epistasis : public pbo_base
            {
            public:

                OneMax_Epistasis(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
                    : pbo_base("OneMax_Epistasis", instance_id)
                {
                    set_number_of_variables(dimension);
                }
                void customize_optimal() {
                    set_optimal(get_number_of_variables());
                }

                double internal_evaluate(const std::vector<int>& x) {
	                auto new_variables = utils::epistasis(x, 4);
                    auto n = new_variables.size();
	                auto result = 0;
                    for (auto i = 0; i != n; ++i) {
                        result += new_variables[i];
                    }
                    return (double)result;
                }

                static OneMax_Epistasis* create(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
                    return new OneMax_Epistasis(instance_id, dimension);
                }
            };
        }
    }
}
