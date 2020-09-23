/// \file f_weierstrass.hpp
/// \brief cpp file for class Weierstrass.
///
/// A detailed file description.
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
			class Weierstrass : public bbob_base
			{
				static constexpr int summands = 10;
				const double condition = 100.0;
				double f0;
				double ak[summands];
				double bk[summands];
			public:
				Weierstrass(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(16, "Weierstrass", instance_id),
					f0(0.0), ak{0.0}, bk{0.0}
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
					transformation::coco::bbob2009_compute_rotation(rot1, rseed + 1000000, n);
					transformation::coco::bbob2009_compute_rotation(rot2, rseed, n);
					for (auto i = 0; i < n; ++i)
					{
						b[i] = 0.0;
						for (auto j = 0; j < n; ++j)
						{
							M[i][j] = 0.0;
							for (auto k = 0; k < n; ++k)
							{
								const auto base = 1.0 / sqrt(condition);
								const auto exponent = 1.0 * static_cast<int>(k) / (static_cast<double>(static_cast<
										long>(n)) - 1.0
								);
								M[i][j] += rot1[i][k] * pow(base, exponent) * rot2[k][j];
							}
						}
					}
					transformation::coco::bbob2009_copy_rotation_matrix(rot1, M1, b1, n);

					f0 = 0.0;
					for (auto i = 0; i < summands; ++i)
					{
						ak[i] = pow(0.5, static_cast<double>(i));
						bk[i] = pow(3., static_cast<double>(i));
						f0 += ak[i] * cos(2 * transformation::coco::coco_pi * bk[i] * 0.5);
					}
					transformation::coco::data::M1 = M1;
					transformation::coco::data::b1 = b1;
					transformation::coco::data::penalty_factor = 10.0 / static_cast<double>(n);
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					auto n = x.size();
					size_t i, j;
					auto result = 0.0;
					for (i = 0; i < n; ++i)
					{
						for (j = 0; j < summands; ++j)
						{
							result += cos(2 * transformation::coco::coco_pi * (x[i] + 0.5) * bk[j]) * ak[j];
						}
					}

					return 10.0 * pow(result / static_cast<double>(static_cast<long>(n)) - f0, 3.0);
				}

				static Weierstrass* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                   int dimension = DEFAULT_DIMENSION)
				{
					return new Weierstrass(instance_id, dimension);
				}
			};
		}
	}
}
