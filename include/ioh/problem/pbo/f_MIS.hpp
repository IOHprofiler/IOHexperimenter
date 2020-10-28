/// \file f_MIS.hpp
/// \brief cpp file for class f_MIS.
///
/// A detailed file description.
///
/// \author Naama Horesh and Furong Ye
/// \date 2019-06-27
#pragma once
#include "pbo_base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace pbo
		{
			class MIS : public pbo_base
			{
			public:
				MIS(int instance_id = IOH_DEFAULT_INSTANCE, int dimension = IOH_DEFAULT_DIMENSION)
					: pbo_base("MIS", instance_id)
				{
					set_number_of_variables(dimension);
				}

				int isEdge(int i, int j, size_t problem_size)
				{
					if (i != problem_size / 2 && j == i + 1)
					{
						return 1;
					}
					if (i <= (static_cast<int>(problem_size) / 2) - 1 && j == i + (static_cast<int>(problem_size) / 2) +
						1)
					{
						return 1;
					}
					if (i <= (static_cast<int>(problem_size) / 2) && i >= 2 && j == i + (static_cast<int>(problem_size)
						/ 2) - 1)
					{
						return 1;
					}
					return 0;
				}


				double internal_evaluate(const std::vector<int>& x) override
				{
					int i, j, index;
					auto n = x.size();
					auto result = 0;
					auto num_of_ones = 0;
					auto sum_edges_in_the_set = 0;
					auto number_of_variables_even = static_cast<int>(n);
					std::vector<int> ones_array(number_of_variables_even + 1);

					if (n % 2 != 0)
					{
						number_of_variables_even = static_cast<int>(n) - 1;
					}

					for (index = 0; index < number_of_variables_even; index++)
					{
						if (x[index] == 1)
						{
							ones_array[num_of_ones] = index;
							num_of_ones += 1;
						}
					}

					for (i = 0; i < num_of_ones; i++)
					{
						for (j = i + 1; j < num_of_ones; j++)
						{
							if (isEdge(ones_array[i] + 1, ones_array[j] + 1,
							           static_cast<size_t>(number_of_variables_even)) == 1)
							{
								sum_edges_in_the_set += 1;
							}
						}
					}
					result = num_of_ones - (number_of_variables_even * sum_edges_in_the_set);
					return static_cast<double>(result);
				}

				static MIS* create(int instance_id = IOH_DEFAULT_INSTANCE, int dimension = IOH_DEFAULT_DIMENSION)
				{
					return new MIS(instance_id, dimension);
				}
			};
		};
	}
}
