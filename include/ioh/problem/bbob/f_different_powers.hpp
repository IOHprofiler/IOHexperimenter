/// \file f_Different_Powers.hpp
/// \brief cpp file for class f_Different_Powers.
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
			class Different_Powers : public bbob_base
			{
			public:
				Different_Powers(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(14, "Different_Powers", instance_id)
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
				
				void variables_transformation(std::vector<double>& x, const int transformation_id,
					const int instance_id) override
				{
					transformation::coco::transform_vars_shift_evaluate_function(x, xopt_);
					transformation::coco::transform_vars_affine_evaluate_function(x, M_, b_);
				}
				

				double internal_evaluate(const std::vector<double>& x) override
				{
					auto sum = 0.0;
					auto n = x.size();

					for (size_t i = 0; i < n; ++i)
					{
						auto exponent = 2.0 + (4.0 * static_cast<double>(static_cast<long>(i))) / (static_cast<double>
							(static_cast<long>(n)) - 1.0);
						sum += pow(fabs(x[i]), exponent);
					}
					return sqrt(sum);
				}

				static Different_Powers* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                        int dimension = DEFAULT_DIMENSION)
				{
					return new Different_Powers(instance_id, dimension);
				}
			};
		}
	}
}
