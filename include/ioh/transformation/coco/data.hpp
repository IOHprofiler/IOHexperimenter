#pragma once
#include <vector>

namespace ioh
{
	namespace transformation
	{
		namespace coco
		{
			struct data
			{
				inline static std::vector<double> raw_x;
				inline static std::vector<double> xopt;
				inline static std::vector<double> tmp1;
				inline static std::vector<double> tmp2;
				inline static double fopt;
				inline static double penalty_factor;
				inline static double factor;
				inline static double lower_bound;
				inline static double upper_bound;
				inline static std::vector<std::vector<double>> M;
				inline static std::vector<double> b;
				inline static std::vector<std::vector<double>> M1;
				inline static std::vector<double> b1;
				inline static std::vector<std::vector<double>> rot1;
				inline static std::vector<std::vector<double>> rot2;
				inline static std::vector<double> datax;
				inline static std::vector<double> dataxx;
				inline static std::vector<double> minus_one;
				inline static double condition;
				inline static long rseed;
			};
		}
	}
}
