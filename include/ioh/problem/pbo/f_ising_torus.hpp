/// \file f_ising_torus.hpp
/// \brief cpp file for class f_ising_torus.
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
			class Ising_Torus : public pbo_base
			{
			public:
				Ising_Torus(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: pbo_base("Ising_Torus", instance_id)
				{
					set_best_variables(1);
					set_number_of_variables(dimension);
				}

				int modulo_ising_torus(int x, int N)
				{
					return (x % N + N) % N;
				}

				double internal_evaluate(const std::vector<int>& x) override
				{
					auto n = x.size();
					int neig;
					auto result = 0.0;
					int neighbors[2];
					auto lattice_size = static_cast<int>(sqrt(static_cast<double>(n)));

					if (floor(sqrt(static_cast<double>(n))) != sqrt(static_cast<double>(n)))
					{
						common::log::error("Number of parameters in the Ising square problem must be a square number");
					}

					for (auto i = 0; i < lattice_size; ++i)
					{
						for (auto j = 0; j < lattice_size; ++j)
						{
							neighbors[0] = x[modulo_ising_torus(i + 1, lattice_size) * lattice_size + j];
							neighbors[1] = x[lattice_size * i + modulo_ising_torus((j + 1), lattice_size)];
							for (neig = 0; neig < 2; neig++)
							{
								result += (x[lattice_size * i + j] * neighbors[neig]) - ((1 - x[i * lattice_size + j]) *
									(1 - neighbors[neig]));
							}
						}
					}

					return result;
				}

				static Ising_Torus* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                   int dimension = DEFAULT_DIMENSION)
				{
					return new Ising_Torus(instance_id, dimension);
				}
			};
		}
	}
}
