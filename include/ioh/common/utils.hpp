#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>

namespace ioh
{
	namespace common
	{
		enum class optimization_type { minimization = 0, maximization = 1 };


		namespace log
		{
			void error(std::string error_info);

			void warning(std::string warning_info);

			void info(std::string log_info);

			void info(std::string log_info, std::ofstream& log_stream);
		}


		template <typename T>
		void copy_vector(const std::vector<T> v1, std::vector<T>& v2)
		{
			v2.assign(v1.begin(), v1.end());
		}

		/// bool compare_vector(std::vector<valueType> &v1, std::vector<valueType> v2)
		///
		/// Return 'true' if all elements in two vectors are the same.
		template <typename T>
		bool compare_vector(const std::vector<T>& v1, const std::vector<T>& v2)
		{
			int n = v1.size();
			if (n != v2.size())
			{
				log::error("Two compared vectors must be with the same size\n");
				return false;
			}
			for (int i = 0; i != n; ++i)
			{
				if (v1[i] != v2[i])
				{
					return false;
				}
			}
			return true;
		}

		/// \fn bool compare_objectives(std::vector<valueType> &v1, std::vector<valueType> v2,  const IOH_optimization_type optimization_type)
		///
		/// Return true if values of vl's elements are better than v2's in each index.
		/// set as maximization if optimization_type = 1, otherwise minimization.
		/// This is used to compare to objectives vector, details needs to be discussed
		/// for multi-objective optimization.
		template <typename T>
		bool compare_objectives(const std::vector<T>& v1, const std::vector<T>& v2,
		                        const optimization_type optimization_type)
		{
			int n = v1.size();
			if (n != v2.size())
			{
				log::error("Two compared objective vector must be with the same size\n");
				return false;
			}
			if (optimization_type == optimization_type::maximization)
			{
				for (int i = 0; i != n; ++i)
				{
					if (v1[i] <= v2[i])
					{
						return false;
					}
				}
				return true;
			}
			for (int i = 0; i != n; ++i)
			{
				if (v1[i] >= v2[i])
				{
					return false;
				}
			}
			return true;
		}

		/// \fn bool compare_objectives(std::vector<valueType> &v1, std::vector<valueType> v2, const IOH_optimization_type optimization_type)
		///
		/// Return true if values of vl's elements are better than v2's in each index.
		/// set as maximization if optimization_type = 1, otherwise minimization.
		/// This is used to compare to objectives vector, details needs to be discussed
		/// for multi-objective optimization.
		template <typename T>
		bool compare_objectives(const T v1, const T v2, const optimization_type optimization_type)
		{
			if (optimization_type == optimization_type::maximization)
				return v1 > v2;
			return v1 < v2;
		}

		template <typename T>
		std::string _toString(const T v)
		{
			std::ostringstream ss;
			ss << v;
			return ss.str();
		}
	}
}
