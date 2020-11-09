/// \file f_nk_landscapes.hpp
/// \brief cpp file for class f_nk_landscapes.
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
			class NK_Landscapes : public pbo_base
			{
			public:
				std::vector<std::vector<double>> F;
				std::vector<std::vector<int>> E;
				int k = 1;
			  /**
				 * \brief Construct a new NK_Landscapes object. Definition refers to https://doi.org/10.1007/978-3-030-58115-2_49 
				 * 
				 * \param instance_id The instance number of a problem, which controls the transformation
				 * performed on the original problem.
				 * \param dimension The dimensionality of the problem to created, 4 by default.
				 **/
				NK_Landscapes(int instance_id = IOH_DEFAULT_INSTANCE, int dimension = IOH_DEFAULT_DIMENSION)
					: pbo_base(25, "NK_Landscapes", instance_id)
				{
					set_number_of_variables(dimension);
				}

				void set_n_k(const int n, const int k)
				{
					E.clear();
					F.clear();
					this->k = k;
					if (k > n)
					{
						common::log::error("NK_Landscapes, k > n");
					}
					std::vector<double> rand_vec;
					for (auto i = 0; i != n; ++i)
					{
						common::Random::uniform(static_cast<size_t>(k), static_cast<long>(k * (i + 1)), rand_vec);

						std::vector<int> sampled_number;
						std::vector<int> population;
						for (auto i = 0; i < n; ++i)
						{
							population.push_back(i);
						}

						int temp, randPos;
						for (auto i = n - 1; i > 0; --i)
						{
							randPos = static_cast<int>(floor(rand_vec[n - 1 - i] * (i + 1)));
							temp = population[i];
							population[i] = population[randPos];
							population[randPos] = temp;
							sampled_number.push_back(population[i]);
							if (n - i - 1 == k - 1)
							{
								break;
							}
						}
						if (n == k)
						{
							sampled_number.push_back(population[0]);
						}
						E.push_back(sampled_number);
					}
					for (auto i = 0; i != n; ++i)
					{
						common::Random::uniform(static_cast<size_t>(pow(2, k + 1)),
						                        static_cast<long>(k * (i + 1) * 2), rand_vec);
						F.push_back(rand_vec);
					}
				}

				void prepare_problem() override
				{
					set_n_k(get_number_of_variables(), k);
				}

				double internal_evaluate(const std::vector<int>& x) override
				{
					auto n = x.size();
					double result = 0;
					size_t index;
					for (auto i = 0; i != n; ++i)
					{
						index = x[i];
						for (auto j = 0; j != k; ++j)
						{
							index = index + static_cast<size_t>(pow(2, (j + 1)) * x[E[i][j]]);
						}
						result += F[i][index];
					}

					result = result / static_cast<double>(n);
					return -result;
				}

				static NK_Landscapes* create(int instance_id = IOH_DEFAULT_INSTANCE,
				                             int dimension = IOH_DEFAULT_DIMENSION)
				{
					return new NK_Landscapes(instance_id, dimension);
				}
			};
		}
	}
}
