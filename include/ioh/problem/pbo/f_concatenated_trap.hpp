/// \file f_Concatenated_Trap.hpp
/// \brief cpp file for class f_concatenated_trap.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-12-01
#pragma once
#include "pbo_base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace pbo
		{
			class Concatenated_Trap : public pbo_base
			{
				int k = 5;
			public:

				Concatenated_Trap(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: pbo_base("Concatenated_Trap", instance_id)
				{
					set_best_variables(1);
					set_number_of_variables(dimension);
					set_optimal(static_cast<double>(dimension));
				}

				double internal_evaluate(const std::vector<int>& x) override
				{
					auto n = x.size();
					auto result = 0.0;
					double block_result;

					auto m = n / k;
					for (size_t i = 1; i <= m; ++i)
					{
						block_result = 0.0;
						for (auto j = i * k - k; j != i * k; ++j)
						{
							block_result += x[j];
						}
						if (block_result == k)
						{
							result += 1;
						}
						else
						{
							result += (static_cast<double>(k - 1) - block_result) / static_cast<double>(k);
						}
					}
					auto remain_k = n - m * k;
					if (remain_k != 0)
					{
						block_result = 0.0;
						// TODO: check this, only with braces this works
						for (auto j = m * (k - 1); j != n; ++j)
						{
							block_result += x[j];
						}
						if (block_result == remain_k)
						{
							result += 1;
						}
						else
						{
							result += (static_cast<double>(remain_k - 1 - block_result) / static_cast<double>(remain_k)
							);
						}
					}

					return result;
				}

				static Concatenated_Trap* create(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
				{
					return new Concatenated_Trap(instance_id, dimension);
				}
			};
		}
	}
}
