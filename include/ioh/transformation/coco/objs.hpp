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
			static void transform_obj_norm_by_dim_evaluate(const int dimension, std::vector<double>& y)
			{
				y[0] *= bbob2009_fmin(1, 40. / static_cast<double>(dimension));
				/* Wassim: might want to use a function (with no 40) here that we can put in a helpers file */
			}

			static void transform_obj_oscillate_evaluate(std::vector<double>& y)
			{
				static const auto factor = 0.1;
				size_t i;
				auto number_of_objectives = y.size();

				for (i = 0; i < number_of_objectives; i++)
				{
					if (y[i] != 0)
					{
						double log_y;
						log_y = log(fabs(y[i])) / factor;
						if (y[i] > 0)
						{
							y[i] = pow(exp(log_y + 0.49 * (sin(log_y) + sin(0.79 * log_y))), factor);
						}
						else
						{
							y[i] = -pow(exp(log_y + 0.49 * (sin(0.55 * log_y) + sin(0.31 * log_y))), factor);
						}
					}
				}
			}

			/**
			 * @brief Evaluates the transformation.
			 */
			static void transform_obj_penalize_evaluate(const std::vector<double>& x, const double lower_bounds,
			                                            const double upper_bounds, const double factor,
			                                            std::vector<double>& y)
			{
				auto penalty = 0.0;
				size_t i;
				auto number_of_objectives = y.size();
				auto n = x.size();

				for (i = 0; i < n; ++i)
				{
					const auto c1 = x[i] - upper_bounds;
					const auto c2 = lower_bounds - x[i];
					if (c1 > 0.0)
					{
						penalty += c1 * c1;
					}
					else if (c2 > 0.0)
					{
						penalty += c2 * c2;
					}
				}

				for (i = 0; i < number_of_objectives; ++i)
				{
					y[i] += factor * penalty;
				}
			}


			static void transform_obj_power_evaluate(std::vector<double>& y, const double exponent)
			{
				size_t i;
				auto number_of_objectives = y.size();

				for (i = 0; i < number_of_objectives; i++)
				{
					y[i] = pow(y[i], exponent);
				}
			}

			static void transform_obj_scale_evaluate_function(std::vector<double>& y, const double factor)
			{
				size_t i;
				auto number_of_objectives = y.size();


				for (i = 0; i < number_of_objectives; i++)
					y[i] *= factor;
			}

			static void transform_obj_shift_evaluate_function(std::vector<double>& y, const double offset)
			{
				size_t i;
				auto number_of_objectives = y.size();

				for (i = 0; i < number_of_objectives; i++)
					y[i] += offset;
			}

		
			static void coco_tranformation_objs(const std::vector<double>& x, std::vector<double>& y,
			                                    const int problem_id)
			{
				if (problem_id == 1)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 2)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 3)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 4)
				{
					/* ignore large-scale test */
					transform_obj_shift_evaluate_function(y, data::fopt);
					transform_obj_penalize_evaluate(data::raw_x, data::lower_bound, data::upper_bound,
					                                data::penalty_factor, y);
				}
				else if (problem_id == 5)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 6)
				{
					transform_obj_oscillate_evaluate(y);
					transform_obj_power_evaluate(y, 0.9);
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 7)
				{
				}
				else if (problem_id == 8)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 9)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 10)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 11)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 12)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 13)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 14)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 15)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 16)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
					transform_obj_penalize_evaluate(data::raw_x, data::lower_bound, data::upper_bound,
					                                data::penalty_factor, y);
				}
				else if (problem_id == 17)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
					transform_obj_penalize_evaluate(data::raw_x, data::lower_bound, data::upper_bound,
					                                data::penalty_factor, y);
				}
				else if (problem_id == 18)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
					transform_obj_penalize_evaluate(data::raw_x, data::lower_bound, data::upper_bound,
					                                data::penalty_factor, y);
				}
				else if (problem_id == 19)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 20)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 21)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 22)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else if (problem_id == 23)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
					transform_obj_penalize_evaluate(data::raw_x, data::lower_bound, data::upper_bound,
					                                data::penalty_factor, y);
				}
				else if (problem_id == 24)
				{
					transform_obj_shift_evaluate_function(y, data::fopt);
				}
				else
				{
				}
			}
		}
	}
}
