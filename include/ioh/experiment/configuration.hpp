#pragma once

#include <algorithm>
#include "ioh/common.hpp"

namespace ioh
{
	namespace experiment
	{
		static int max_number_of_problem;
		static int max_dimension;

		class _dict
		{
		public:
			_dict() = default;
			~_dict() = default;

			int n = 0;
			size_t size;
			std::vector<std::string> section;
			std::vector<std::string> value;
			std::vector<std::string> key;
		};

		typedef enum _LINE_
		{
			EMPTY,
			COMMENT,
			SECTION,
			VALUE,
			CON_ERROR
		} linecontent;

		/// A class of configuration files, to be used in experimenter.
		class configuration
		{
			std::string cfgFile = "configuration.ini";
			std::string suite_name;
			std::vector<int> problem_id;
			std::vector<int> instance_id;
			std::vector<int> dimension;
			std::string output_directory;
			std::string result_folder;
			std::string algorithm_info;
			std::string algorithm_name;
			bool complete_triggers;
			bool update_triggers;
			std::vector<int> base_evaluation_triggers;
			int number_target_triggers;
			int number_interval_triggers;
		public:
			configuration() = default;

			int set_dict(_dict& dict, std::string section, std::string key, std::string value)
			{
				if (dict.n > 0)
				{
					for (auto i = 0; i < dict.n; ++i)
					{
						if (key.length() == 0)
						{
							continue;
						}
						if (value.length() == 0)
						{
							continue;
						}
						if (dict.key[i].length() == 0)
						{
							continue;
						}
						if (dict.section[i].length() == 0)
						{
							continue;
						}
						if ((key == dict.key[i]) && (section == dict.section[i]))
						{
							if (dict.value[i].length() != 0)
							{
								std::cout << "Multi setting of key(" << key << ") or section(" << section << ")\n";
								return -1;
							}
						}
					}
				}
				dict.section.push_back(common::strstrip(section));
				dict.key.push_back(common::strstrip(key));
				dict.value.push_back(common::strstrip(value));
				dict.n += 1;
				return 0;
			}

			std::string get_dict_String(_dict dict, std::string section, std::string key)
			{
				{
					if (key.length() == 0)
					{
						std::cout << "EMPTY KEY INPUT.\n";
					}
					if (section.length() == 0)
					{
						std::cout << "EMPTY SECTION INPUT.\n";
					}
					for (auto i = 0; i < dict.n; ++i)
					{
						if (dict.key[i].empty() || dict.section[i].empty())
							continue;
						if ((key == dict.key[i]) && (section == dict.section[i]))
							return dict.value[i];
					}
					std::cout << "Can not find the corresponding configuration for key: "
						<< key << " in section : " << section << std::endl;

					return nullptr;
				}
			}

			std::vector<int> get_dict_int_vector(_dict dict, std::string section, std::string key, int _min, int _max)
			{
				size_t i;
				std::vector<int> result;
				if (key.length() == 0)
				{
					std::cout << "EMPTY KEY INPUT.\n";
				}
				if (section.length() == 0)
				{
					std::cout << "EMPTY SECTION INPUT.\n";
				}
				for (i = 0; i < dict.n; ++i)
				{
					if (dict.key[i].length() == 0)
					{
						continue;
					}
					if (dict.section[i].length() == 0)
					{
						continue;
					}
					if (key == dict.key[i] && section == dict.section[i])
					{
						result = common::get_int_vector_parse_string(dict.value[i], _min, _max);
						return result;
					}
				}
				std::cout << "Can not find the corresponding configuration for key: " << key << " in section : " << section
					<< "\n";
				return result;
			}

			int get_dict_Int(_dict dict, std::string section, std::string key)
			{
				int result;
				std::string str;
				str = get_dict_String(dict, section, key);
				result = std::stoi(str);
				return result;
			}

			bool get_dict_bool(_dict dict, std::string section, std::string key)
			{
				bool result = false;
				std::string str;
				str = get_dict_String(dict, section, key);
				transform(str.begin(), str.end(), str.begin(), tolower);
				if (str == "true")
					result = true;
				return result;
			}

			linecontent add_Line(std::string input_line, std::string& section, std::string& key, std::string& value)
			{
				linecontent content;
				std::string line;
				size_t len;
				char tempkey[MAXKEYNUMBER];
				char tempvalue[MAXKEYNUMBER];
				char tempsection[MAXKEYNUMBER];

				line = common::strstrip(input_line);
				len = line.length();

				if (line.empty())
				{
					content = EMPTY;
				}
				if (line[0] == '#' || line[0] == ';')
				{
					content = COMMENT;
				}
				else if (line[0] == '[' && line[len - 1] == ']')
				{
					sscanf(line.c_str(), "[%[^]]", tempsection);
					section = tempsection;
					content = SECTION;
				}
				else if (sscanf(line.c_str(), "%[^=] = \"%[^\"]", tempkey, tempvalue) == 2 || sscanf(
					line.c_str(), "%[^=] = '%[^\']", tempkey, tempvalue) == 2)
				{
					value = tempvalue;
					key = tempkey;
					content = VALUE;
				}
				else if (sscanf(line.c_str(), "%[^=] = %[^;#]", tempkey, tempvalue) == 2)
				{
					value = tempvalue;
					key = tempkey;
					content = VALUE;
				}
				else
				{
					content = CON_ERROR;
				}
				return content;
			}
			

			_dict load(std::string filename)
			{
				std::ifstream fp(filename.c_str());

				std::string line;
				std::string section;
				std::string key;
				std::string value;

				int len;
				_dict dict;
				linecontent lc;

				if (!fp.is_open())
				{
					std::cout << "Cannot open file " << filename << std::endl;
				}

				while (!fp.eof())
				{
					getline(fp, line);
					len = line.length() - 1;
					if (len <= 0)
					{
						continue;
					}

					lc = add_Line(line, section, key, value);
					switch (lc)
					{
					case EMPTY:
						break;
					case COMMENT:
						break;
					case SECTION:
						common::strstrip(section);
						this->set_dict(dict, section, key, value);
						break;
					case VALUE:
						common::strstrip(key);
						this->set_dict(dict, section, key, value);
						break;
					case CON_ERROR:
						std::cout << "There is an error for line: \" " << line << "\"";
						break;
					}
					if (lc == CON_ERROR)
					{
						break;
					}
				}
				fp.close();
				return dict;
			}
			

			void readcfg(std::string filename)
			{
				_dict dict;
				dict = load(filename);
				suite_name = get_dict_String(dict, "suite", "suite_name");
				if (suite_name == "BBOB")
				{
					max_number_of_problem = 24;
					max_dimension = 100;
				}
				else if (suite_name == "PBO")
				{
					max_dimension = 20000;
					max_number_of_problem = 23;
				}
				problem_id = get_dict_int_vector(dict, "suite", "problem_id", 1, max_number_of_problem);
				instance_id = get_dict_int_vector(dict, "suite", "instance_id", 1, 100);
				dimension = get_dict_int_vector(dict, "suite", "dimension", 1, max_dimension);

				output_directory = get_dict_String(dict, "logger", "output_directory");
				result_folder = get_dict_String(dict, "logger", "result_folder");
				algorithm_info = get_dict_String(dict, "logger", "algorithm_info");
				algorithm_name = get_dict_String(dict, "logger", "algorithm_name");

				complete_triggers = get_dict_bool(dict, "observer", "complete_triggers");
				update_triggers = get_dict_bool(dict, "observer", "update_triggers");
				base_evaluation_triggers = get_dict_int_vector(dict, "observer", "base_evaluation_triggers", 0, 10);
				number_target_triggers = get_dict_Int(dict, "observer", "number_target_triggers");
				number_interval_triggers = get_dict_Int(dict, "observer", "number_interval_triggers");
			}

			std::string get_suite_name()
			{
				return this->suite_name;
			}

			std::vector<int> get_problem_id()
			{
				return this->problem_id;
			}

			std::vector<int> get_instance_id()
			{
				return this->instance_id;
			}

			std::vector<int> get_dimension()
			{
				return this->dimension;
			}

			std::string get_output_directory()
			{
				return this->output_directory;
			}

			std::string get_result_folder()
			{
				return this->result_folder;
			}

			std::string get_algorithm_info()
			{
				return this->algorithm_info;
			}

			std::string get_algorithm_name()
			{
				return this->algorithm_name;
			}

			bool get_complete_triggers()
			{
				return this->complete_triggers;
			}

			bool get_update_triggers()
			{
				return this->update_triggers;
			}

			std::vector<int> get_base_evaluation_triggers()
			{
				return this->base_evaluation_triggers;
			}

			int get_number_target_triggers()
			{
				return this->number_target_triggers;
			}

			int get_number_interval_triggers()
			{
				return this->number_interval_triggers;
			}
		};
	}
}
