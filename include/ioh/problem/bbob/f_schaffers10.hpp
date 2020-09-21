/// \file f_schaffers10.hpp
/// \brief cpp file for class Schaffers10.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_schaffers.c"
///
/// \author Furong Ye
/// \date 2019-09-12
#pragma once
#include "bbob_base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace bbob
		{
			class Schaffers10 : public bbob_base
			{
				const double conditioning = 10;
			public:
				Schaffers10(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(17, "Schaffers10", instance_id)
				{
					set_number_of_variables(dimension);
				}

				void prepare_bbob_problem(std::vector<double>& xopt, std::vector<std::vector<double>>& M,
				                          std::vector<double>& b, std::vector<std::vector<double>>& rot1,
				                          std::vector<std::vector<double>>& rot2,
				                          const long rseed, const long n
				) override
				{
					std::vector<std::vector<double>> M1(n, std::vector<double>(n));
					std::vector<double> b1(n);

					transformation::coco::bbob2009_compute_xopt(xopt, rseed, n);

					/* compute M and b */
					transformation::coco::bbob2009_compute_rotation(rot1, rseed + 1000000, n);
					transformation::coco::bbob2009_compute_rotation(rot2, rseed, n);
					for (int i = 0; i < n; ++i)
					{
						b[i] = 0.0;
						for (int j = 0; j < n; ++j)
						{
							double exponent = 1.0 * static_cast<int>(i) / (static_cast<double>(static_cast<long>(n)) -
								1.0);
							M[i][j] = rot2[i][j] * pow(sqrt(conditioning), exponent);
						}
					}
					transformation::coco::bbob2009_copy_rotation_matrix(rot1, M1, b1, n);

					transformation::coco::data::M1 = M1;
					transformation::coco::data::b1 = b1;
					transformation::coco::data::penalty_factor = 10.0;
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					size_t n = x.size();
					double result = 0.0;
					for (size_t i = 0; i < n - 1; ++i)
					{
						const double tmp = x[i] * x[i] + x[i + 1] * x[i + 1];
						if (std::isinf(tmp) && std::isnan(sin(50.0 * pow(tmp, 0.1)))) /* sin(inf) -> nan */
							/* the second condition is necessary to pass the integration tests under Windows and Linux */
							return tmp;
						result += pow(tmp, 0.25) * (1.0 + pow(sin(50.0 * pow(tmp, 0.1)), 2.0));
					}
					return pow(result / (static_cast<double>(static_cast<long>(n)) - 1.0), 2.0);
				}

				static Schaffers10* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                   int dimension = DEFAULT_DIMENSION)
				{
					return new Schaffers10(instance_id, dimension);
				}
			};
		}
	}
}
