/// \file f_Discus.hpp
/// \brief cpp file for class f_Discus.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_discus.c"
///
/// \author Furong Ye
/// \date 2019-09-10
#pragma once
#include "bbob_base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace bbob
		{
			class Discus : public bbob_base
			{
			public:
				Discus(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(11, "Discus", instance_id)
				{
					set_number_of_variables(dimension);
				}

				void prepare_bbob_problem(std::vector<double>& xopt, std::vector<std::vector<double>>& M,
				                          std::vector<double>& b, std::vector<std::vector<double>>& rot1,
				                          std::vector<std::vector<double>>& rot2,
				                          const long rseed, const long n
				) override
				{
					transformation::coco::bbob2009_compute_xopt(xopt, rseed, n);
					transformation::coco::bbob2009_compute_rotation(rot1, rseed + 1000000, n);
					transformation::coco::bbob2009_copy_rotation_matrix(rot1, M, b, n);
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					static const auto condition = 1.0e6;
					auto result = condition * x[0] * x[0];
					for (size_t i = 1; i < x.size(); ++i)
					{
						result += x[i] * x[i];
					}
					return result;
				}

				static Discus* createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
				{
					return new Discus(instance_id, dimension);
				}
			};
		}
	}
}
