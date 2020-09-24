/// \file f_bent_cigar.hpp
/// \brief cpp file for class f_Bent_Cigar.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_bent_cigar.c"
///
/// \author Furong Ye
/// \date 2019-09-10
#include "bbob_base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace bbob
		{
			class Bent_Cigar : public bbob_base
			{
			public:
				Bent_Cigar(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(12, "Bent_Cigar", instance_id)
				{
					set_number_of_variables(dimension);
				}

				void prepare_bbob_problem(std::vector<double>& xopt, std::vector<std::vector<double>>& M,
				                          std::vector<double>& b, std::vector<std::vector<double>>& rot1,
				                          std::vector<std::vector<double>>& rot2,
				                          const long rseed, const long n
				) override
				{
					transformation::coco::bbob2009_compute_xopt(xopt, rseed + 1000000, n);
					transformation::coco::bbob2009_compute_rotation(rot1, rseed + 1000000, n);
					transformation::coco::bbob2009_copy_rotation_matrix(rot1, M, b, n);
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					static const auto condition = 1.0e6;
					auto result = x[0] * x[0];
					for (size_t i = 1; i < x.size(); ++i)
					{
						result += condition * x[i] * x[i];
					}
					return result;
				}
				
				void variables_transformation(std::vector<double>& x, const int transformation_id,
				                              const int instance_id) override
				{
					transformation::coco::transform_vars_shift_evaluate_function(x, xopt_);
					transformation::coco::transform_vars_affine_evaluate_function(x, M_, b_);
					transformation::coco::transform_vars_asymmetric_evaluate_function(x, 0.5);
					transformation::coco::transform_vars_affine_evaluate_function(x, M_, b_);
				}

				static Bent_Cigar* createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
				{
					return new Bent_Cigar(instance_id, dimension);
				}
			};
		}
	}
}
