/// \file IOHprofiler_transformation.hpp
/// \brief Header file for the class IOHprofiler_transformation.
///
/// \author Furong Ye
#pragma once

#include <vector>
#include <string>
#include "ioh/common/random.hpp"
#include "coco.hpp"


namespace ioh
{
	namespace problem
	{
	namespace transformation
	{
		namespace methods
		{
			/// \fn xor_compute(const int x1, const int x2)
			///
			/// Exclusive or operation on x1, x2. x1 and x2 should either 0 or 1.
			static int xor_compute(int x1, int x2);

			/// \fn transform_vars_xor(std::vector<int> &x, const int seed)
			///
			/// Applying xor operations on x with a uniformly random bit string.
			void transform_vars_xor(std::vector<int>& x, int seed);

			static int sigma_compute(const std::vector<int>& x, int pos);

			/// \fn transform_vars_sigma(std::vector<int> &x, const int seed)
			///
			/// Disrupting the order of x.
			void transform_vars_sigma(std::vector<int>& x, int seed);

			/// \fn void transform_obj_scale(std::vector<double> &y, const int seed)
			/// \brief transformation 'a * f(x)'.
			void transform_obj_scale(std::vector<double>& y, int seed);

			/// \fn void transform_obj_shift(std::vector<double> &y, const int seed)
			/// \brief transformation 'f(x) + b'.
			void transform_obj_shift(std::vector<double>& y, int seed);

			/// \fn void transform_obj_scale(std::vector<double> &y, const int seed)
			/// \brief transformation 'a * f(x)'.
			void transform_obj_scale(double& y, int seed);

			/// \fn void transform_obj_shift(std::vector<double> &y, const int seed)
			/// \brief transformation 'f(x) + b'.
			void transform_obj_shift(double& y, int seed);
		};
	}
}
}