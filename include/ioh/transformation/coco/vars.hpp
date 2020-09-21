#pragma once

#include <vector>
#include "data.hpp"
#include "legacy.hpp"


namespace ioh
{
	namespace transformation
	{
		namespace coco
		{
			static void transform_vars_affine_evaluate_function(std::vector<double>& x,
			                                                    const std::vector<std::vector<double>>& M,
			                                                    const std::vector<double>& b)
			{
				size_t i, j;
				size_t number_of_variables = x.size();
				//double *cons_values;
				//int is_feasible;
				std::vector<double> temp_x = x;
				for (i = 0; i < number_of_variables; ++i)
				{
					/* data->M has problem->number_of_variables columns and inner_problem->number_of_variables rows. */
					x[i] = b[i];
					for (j = 0; j < number_of_variables; ++j)
					{
						x[i] += temp_x[j] * M[i][j];
					}
				}
			}

			static void transform_vars_asymmetric_evaluate_function(std::vector<double>& x, const double beta)
			{
				size_t i;
				double exponent;
				size_t number_of_variables = x.size();
				for (i = 0; i < number_of_variables; ++i)
				{
					if (x[i] > 0.0)
					{
						exponent = 1.0
							+ ((beta * static_cast<double>(static_cast<long>(i))) / (static_cast<double>(static_cast<
								long>(number_of_variables)) - 1.0)) * sqrt(x[i]);
						x[i] = pow(x[i], exponent);
					}
					else
					{
						x[i] = x[i];
					}
				}
			}

			static void transform_vars_brs_evaluate(std::vector<double>& x)
			{
				size_t i;
				double factor;
				size_t number_of_variables = x.size();

				for (i = 0; i < number_of_variables; ++i)
				{
					/* Function documentation says we should compute 10^(0.5 *
					 * (i-1)/(D-1)). Instead we compute the equivalent
					 * sqrt(10)^((i-1)/(D-1)) just like the legacy code.
					 */
					factor = pow(
						sqrt(10.0),
						static_cast<double>(static_cast<long>(i)) / (static_cast<double>(static_cast<long>(
							number_of_variables)) - 1.0));
					/* Documentation specifies odd indices and starts indexing
					 * from 1, we use all even indices since C starts indexing
					 * with 0.
					 */
					if (x[i] > 0.0 && i % 2 == 0)
					{
						factor *= 10.0;
					}
					x[i] = factor * x[i];
				}
			}

			static void transform_vars_conditioning_evaluate(std::vector<double>& x, const double alpha)
			{
				size_t i;
				size_t number_of_variables = x.size();

				for (i = 0; i < number_of_variables; ++i)
				{
					/* OME: We could precalculate the scaling coefficients if we
					 * really wanted to.
					 */
					x[i] = pow(
							alpha,
							0.5 * static_cast<double>(static_cast<long>(i)) / (static_cast<double>(static_cast<long>(
								number_of_variables)) - 1.0))
						* x[i];
				}
			}

			static void transform_vars_oscillate_evaluate_function(std::vector<double>& x)
			{
				static const double alpha = 0.1;
				size_t number_of_variables = x.size();
				double tmp, base;
				size_t i;

				for (i = 0; i < number_of_variables; ++i)
				{
					if (x[i] > 0.0)
					{
						tmp = log(x[i]) / alpha;
						base = exp(tmp + 0.49 * (sin(tmp) + sin(0.79 * tmp)));
						x[i] = pow(base, alpha);
					}
					else if (x[i] < 0.0)
					{
						tmp = log(-x[i]) / alpha;
						base = exp(tmp + 0.49 * (sin(0.55 * tmp) + sin(0.31 * tmp)));
						x[i] = -pow(base, alpha);
					}
					else
					{
						x[i] = 0.0;
					}
				}
			}

			static void transform_vars_scale_evaluate(std::vector<double>& x, const double factor)
			{
				size_t i;
				size_t number_of_variables = x.size();

				for (i = 0; i < number_of_variables; ++i)
				{
					x[i] = factor * x[i];
				}
			}

			static void transform_vars_shift_evaluate_function(std::vector<double>& x,
			                                                   const std::vector<double>& offset)
			{
				size_t i;
				size_t number_of_variables = x.size();

				for (i = 0; i < number_of_variables; ++i)
				{
					x[i] = x[i] - offset[i];
				}
			}

			static void transform_vars_x_hat_evaluate(std::vector<double>& x, const long seed)
			{
				size_t i;
				size_t number_of_variables = x.size();
				std::vector<double> tmp_x;


				bbob2009_unif(tmp_x, static_cast<long>(number_of_variables), seed);

				for (i = 0; i < number_of_variables; ++i)
				{
					if (tmp_x[i] < 0.5)
					{
						x[i] = -x[i];
					}
					else
					{
						x[i] = x[i];
					}
				}
			}

			static void transform_vars_x_hat_generic_evaluate(std::vector<double>& x,
			                                                  const std::vector<double>& sign_vector)
			{
				size_t i;
				size_t number_of_variables = x.size();

				for (i = 0; i < number_of_variables; ++i)
				{
					x[i] = 2.0 * sign_vector[i] * x[i];
				}
			}

			static void transform_vars_z_hat_evaluate(std::vector<double>& x, const std::vector<double>& xopt)
			{
				size_t i;
				size_t number_of_variables = x.size();
				std::vector<double> temp_x = x;
				x[0] = temp_x[0];

				for (i = 1; i < number_of_variables; ++i)
				{
					x[i] = temp_x[i] + 0.25 * (temp_x[i - 1] - 2.0 * fabs(xopt[i - 1]));
				}
			}


			static void coco_tranformation_vars(std::vector<double>& x, const int problem_id)
			{
				if (problem_id == 1)
				{
					transform_vars_shift_evaluate_function(x, data::xopt);
				}
				else if (problem_id == 2)
				{
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_oscillate_evaluate_function(x);
				}
				else if (problem_id == 3)
				{
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_oscillate_evaluate_function(x);
					transform_vars_asymmetric_evaluate_function(x, 0.2);
					transform_vars_conditioning_evaluate(x, 10.0);
				}
				else if (problem_id == 4)
				{
					data::raw_x = x;
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_oscillate_evaluate_function(x);
					transform_vars_brs_evaluate(x);
				}
				else if (problem_id == 5)
				{
				}
				else if (problem_id == 6)
				{
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_affine_evaluate_function(x, data::M, data::b);
				}
				else if (problem_id == 7)
				{
				}
				else if (problem_id == 8)
				{
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_scale_evaluate(x, data::factor);
					transform_vars_shift_evaluate_function(x, data::minus_one);
				}
				else if (problem_id == 9)
				{
					transform_vars_affine_evaluate_function(x, data::M, data::b);
				}
				else if (problem_id == 10)
				{
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_affine_evaluate_function(x, data::M, data::b);
					transform_vars_oscillate_evaluate_function(x);
				}
				else if (problem_id == 11)
				{
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_affine_evaluate_function(x, data::M, data::b);
					transform_vars_oscillate_evaluate_function(x);
				}
				else if (problem_id == 12)
				{
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_affine_evaluate_function(x, data::M, data::b);
					transform_vars_asymmetric_evaluate_function(x, 0.5);
					transform_vars_affine_evaluate_function(x, data::M, data::b);
				}
				else if (problem_id == 13)
				{
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_affine_evaluate_function(x, data::M, data::b);
				}
				else if (problem_id == 14)
				{
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_affine_evaluate_function(x, data::M, data::b);
				}
				else if (problem_id == 15)
				{
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_affine_evaluate_function(x, data::M1, data::b1);
					transform_vars_oscillate_evaluate_function(x);
					transform_vars_asymmetric_evaluate_function(x, 0.2);
					transform_vars_affine_evaluate_function(x, data::M, data::b);
				}
				else if (problem_id == 16)
				{
					data::raw_x = x;
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_affine_evaluate_function(x, data::M1, data::b1);
					transform_vars_oscillate_evaluate_function(x);
					transform_vars_affine_evaluate_function(x, data::M, data::b);
				}
				else if (problem_id == 17)
				{
					data::raw_x = x;
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_affine_evaluate_function(x, data::M1, data::b1);
					transform_vars_asymmetric_evaluate_function(x, 0.5);
					transform_vars_affine_evaluate_function(x, data::M, data::b);
				}
				else if (problem_id == 18)
				{
					data::raw_x = x;
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_affine_evaluate_function(x, data::M1, data::b1);
					transform_vars_asymmetric_evaluate_function(x, 0.5);
					transform_vars_affine_evaluate_function(x, data::M, data::b);
				}
				else if (problem_id == 19)
				{
					transform_vars_affine_evaluate_function(x, data::M, data::b);
					transform_vars_shift_evaluate_function(x, data::xopt);
				}
				else if (problem_id == 20)
				{
					transform_vars_x_hat_evaluate(x, data::rseed);
					transform_vars_scale_evaluate(x, 2);
					transform_vars_z_hat_evaluate(x, data::xopt);
					transform_vars_shift_evaluate_function(x, data::tmp2);
					transform_vars_conditioning_evaluate(x, 10.0);
					transform_vars_shift_evaluate_function(x, data::tmp1);
					transform_vars_scale_evaluate(x, 100);
				}
				else if (problem_id == 21)
				{
				}
				else if (problem_id == 22)
				{
				}
				else if (problem_id == 23)
				{
					data::raw_x = x;
					transform_vars_shift_evaluate_function(x, data::xopt);
					transform_vars_affine_evaluate_function(x, data::M, data::b);
				}
				else if (problem_id == 24)
				{
				}
				else
				{
				}
			}
		}
	}
}
