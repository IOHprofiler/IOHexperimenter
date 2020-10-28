#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

namespace ioh
{
	namespace common
	{
		enum class optimization_type { minimization = 0, maximization = 1 };

		namespace log
		{
			enum class level
			{
				info, warning, error
			};

			inline level log_level = level::info;
				
			inline void error(const std::string error_info)
			{
				if (log_level <= level::error)
				{
					std::cerr << "IOH_ERROR_INFO : " << error_info << std::endl;
					exit(1);
				}
			}

			inline void warning(const std::string warning_info)
			{
				if (log_level <= level::warning)
					std::cout << "IOH_WARNING_INFO : " << warning_info << std::endl;
			}

			inline void info(const std::string log_info)
			{
				if (log_level <= level::info)
					std::cout << "IOH_LOG_INFO : " << log_info << std::endl;
			}

			inline void info(const std::string log_info, std::ofstream& log_stream)
			{
				if (log_level >= level::info)
					log_stream << "IOH_LOG_INFO : " << log_info << std::endl;
			}
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
			auto n = v1.size();
			if (n != v2.size())
			{
				log::error("Two compared vectors must be with the same size\n");
				return false;
			}
			for (auto i = 0; i != n; ++i)
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
			auto n = v1.size();
			if (n != v2.size())
			{
				log::error("Two compared objective vector must be with the same size\n");
				return false;
			}
			if (optimization_type == optimization_type::maximization)
			{
				for (auto i = 0; i != n; ++i)
				{
					if (v1[i] <= v2[i])
					{
						return false;
					}
				}
				return true;
			}
			for (auto i = 0; i != n; ++i)
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
		std::string to_string(const T v)
		{
			std::ostringstream ss;
			ss << v;
			return ss.str();
		}

		/// \fn std::string strstrip(std::string s)
		/// \brief To erase blanks in the front and end of a string.
		///
		/// \para s string
		/// \return string
		static std::string strstrip(std::string s)
		{
			if (s.empty())
			{
				return s;
			}
			s.erase(0, s.find_first_not_of(' '));
			/// For string line end with '\r' in windows systems.
			s.erase(s.find_last_not_of('\r') + 1);
			s.erase(s.find_last_not_of(' ') + 1);
			return s;
		}

		static std::string to_lower(std::string s)
		{
			std::transform(s.begin(), s.end(), s.begin(), tolower);
			return s;
		}

		/// \fn std::vector<int> get_int_vector_parse_string(std::string input, const int _min, const int _max) {
		/// \brief To get a vector of integer values with a range 'n-m'
		///
		/// Return a vector of integer value. The supported formats are:
		///   '-m', [_min, m]
		///   'n-', [n, _max]
		///   'n-m', [n, m]
		///   'n-x-y-z-m', [n,m]
		/// \para input string
		/// \para _int int
		/// \para _max int
		/// \return std::vector<int>
		static std::vector<int> get_int_vector_parse_string(std::string input, const int _min, const int _max)
		{
			std::vector<std::string> spiltstring;
			std::string tmp;
			int tmpvalue, tmpvalue1;
			std::vector<int> result;

			size_t n = input.size();
			input = strstrip(input);
			for (size_t i = 0; i < n; ++i)
			{
				if (input[i] != ',' && input[i] != '-' && !isdigit(input[i]))
				{
					log::error("The configuration consists invalid characters.");
				}
			}

			std::stringstream raw(input);
			while (getline(raw, tmp, ',')) spiltstring.push_back(tmp);

			n = spiltstring.size();
			for (size_t i = 0; i < n; ++i)
			{
				size_t l = spiltstring[i].size();

				if (spiltstring[i][0] == '-')
				{
					/// The condition beginning with "-m"
					if (i != 0)
					{
						log::error("Format error in configuration.");
					}
					else
					{
						tmp = spiltstring[i].substr(1);
						if (tmp.find('-') != std::string::npos)
						{
							log::error("Format error in configuration.");
						}

						tmpvalue = std::stoi(tmp);

						if (tmpvalue < _min)
						{
							log::error("Input value exceeds lowerbound.");
						}

						for (int value = _min; value <= tmpvalue; ++value)
						{
							result.push_back(value);
						}
					}
				}
				else if (spiltstring[i][spiltstring[i].length() - 1] == '-')
				{
					/// The condition endding with "n-"
					if (i != spiltstring.size() - 1)
						log::error("Format error in configuration.");
					else
					{
						tmp = spiltstring[i].substr(0, spiltstring[i].length() - 1);
						if (tmp.find('-') != std::string::npos)
						{
							log::error("Format error in configuration.");
						}
						tmpvalue = std::stoi(tmp);
						if (tmpvalue > _max)
						{
							log::error("Input value exceeds upperbound.");
						}
						for (int value = _max; value <= tmpvalue; --value)
						{
							result.push_back(value);
						}
					}
				}
				else
				{
					/// The condition with "n-m,n-x-m"
					std::stringstream tempraw(spiltstring[i]);
					std::vector<std::string> tmpvaluevector;
					while (getline(tempraw, tmp, '-'))
					{
						tmpvaluevector.push_back(tmp);
					}
					tmpvalue = std::stoi(tmpvaluevector[0]);
					tmpvalue1 = std::stoi(tmpvaluevector[tmpvaluevector.size() - 1]);
					if (tmpvalue > tmpvalue1)
					{
						log::error("Format error in configuration.");
					}
					if (tmpvalue < _min)
					{
						log::error("Input value exceeds lowerbound.");
					}
					if (tmpvalue1 > _max)
					{
						log::error("Input value exceeds upperbound.");
					}
					for (int value = tmpvalue; value <= tmpvalue1; ++value) result.push_back(value);
				}
			}
			return result;
		}

		class container
		{
			std::unordered_map<std::string,
				std::unordered_map<std::string, std::string>> data;

			static std::string nice(const std::string& key)
			{
				return common::to_lower(common::strstrip(key));
			}
		public:
			void set(const std::string& section, const std::string& key, const std::string& value)
			{
				data[nice(section)][nice(key)] = value;
			}

			[[nodiscard]]
			std::unordered_map<std::string, std::string> get(const std::string& section) const
			{
				const auto iter = data.find(nice(section));
				if (iter != data.end())
					return iter->second;
				std::cout << "Cannot find section: " << section << std::endl;
			}

			[[nodiscard]]
			std::string get(const std::string& section, const std::string& key) const
			{
				auto map = get(section);
				const auto iter = map.find(nice(key));
				if (iter != map.end())
					return iter->second;
				std::cout << "Cannot find key: " << section << std::endl;
				return nullptr;
			}

			int get_int(const std::string& section, const std::string& key) const
			{
				return std::stoi(get(section, key));
			}

			int get_bool(const std::string& section, const std::string& key) const
			{
				return nice(get(section, key)) == "true";
			}

			std::vector<int> get_int_vector(const std::string& section, const std::string& key, const int _min, const int _max) const
			{
				return get_int_vector_parse_string(get(section, key), _min, _max);
			}
		};


		
	}
}
