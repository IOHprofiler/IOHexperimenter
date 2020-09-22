/// \file f_Bueche_Rastrigin.hpp
/// \brief cpp file for class f_Bueche_Rastrigin.
///
/// A detailed file description.
///
/// \author Naama Horesh and Furong Ye
/// \date 2019-06-27
#pragma once
#include "bbob_base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace bbob
		{
			class Bueche_Rastrigin : public bbob_base
			{
			public:
				Bueche_Rastrigin(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(4, "Bueche_Rastrigin", instance_id)
				{
					//TODO: This has to be called inside the child class need we to think of something here
					set_number_of_variables(dimension);
				}

				void prepare_bbob_problem(std::vector<double>& xopt, std::vector<std::vector<double>>& M,
				                          std::vector<double>& b, std::vector<std::vector<double>>& rot1,
				                          std::vector<std::vector<double>>& rot2,
				                          const long rseed, const long n
				) override
				{
					transformation::coco::bbob2009_compute_xopt(xopt, rseed, n);
					for (int i = 0; i < n; i += 2)
					{
						xopt[i] = fabs(xopt[i]);
					}
					transformation::coco::data::penalty_factor = 100.0;
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					double tmp = 0., tmp2 = 0.;
					size_t n = x.size();
					for (size_t i = 0; i < n; ++i)
					{
						tmp += cos(2 * transformation::coco::coco_pi * x[i]);
						tmp2 += x[i] * x[i];
					}
					return 10.0 * (static_cast<double>(static_cast<long>(n)) - tmp) + tmp2 + 0;
				}

				static Bueche_Rastrigin* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                        int dimension = DEFAULT_DIMENSION)
				{
					return new Bueche_Rastrigin(instance_id, dimension);
				}
			};
		}
	}
}
