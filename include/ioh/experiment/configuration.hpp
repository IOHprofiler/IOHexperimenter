#pragma once

#include <filesystem>
#include "ioh/common.hpp"

namespace fs = std::filesystem;

namespace ioh
{
	namespace experiment
	{
		
		/// A class of configuration files, to be used in experimenter.
		class configuration
		{
			std::string config_file = "configuration.ini";
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
			int max_number_of_problem;
			int max_dimension;

			
			common::container data;
		public:
			configuration() = default;
			
			configuration(const std::string& filename) : config_file(filename)
			{
				readcfg(filename);
			}

			void readcfg(const std::string& filename)
			{
				load(filename);
				suite_name = data.get("suite", "suite_name");
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
				problem_id = data.get_int_vector("suite", "problem_id", 1, max_number_of_problem);
				instance_id = data.get_int_vector("suite", "instance_id", 1, 100);
				dimension = data.get_int_vector("suite", "dimension", 1, max_dimension);

				output_directory = data.get("logger", "output_directory");
				result_folder = data.get("logger", "result_folder");
				algorithm_info = data.get("logger", "algorithm_info");
				algorithm_name = data.get("logger", "algorithm_name");

				complete_triggers = data.get_bool("observer", "complete_triggers");
				update_triggers = data.get_bool("observer", "update_triggers");
				base_evaluation_triggers = data.get_int_vector("observer", "base_evaluation_triggers", 0, 10);
				number_target_triggers = data.get_int("observer", "number_target_triggers");
				number_interval_triggers = data.get_int("observer", "number_interval_triggers");
			}

			std::ifstream open_file(const std::string& filename) const
			{
				if (!fs::exists(filename))
					common::log::error("Cannot find file " + filename);

				std::ifstream file(filename.c_str());
				if (!file.is_open())
					common::log::error("Cannot open file " + filename);
				return file;				
			}
			
			void load(const std::string& filename)
			{
				std::string line;
				std::ifstream fp = open_file(filename);

				char key[IOH_MAX_KEYNUMBER];
				char value[IOH_MAX_KEYNUMBER];
				char section[IOH_MAX_KEYNUMBER];
				
				while (getline(fp, line))
				{
					line = common::strstrip(line);
					if (line.empty() || line.front() == '#' || line.front() == ';')
						continue;

					if (line.front() == '[' && line.back() == ']')
						sscanf(line.c_str(), "[%[^]]", section);
					else if (
						sscanf(line.c_str(), "%[^=] = \"%[^\"]", key, value) == 2 
						|| sscanf(line.c_str(), "%[^=] = '%[^\']", key, value) == 2
						|| sscanf(line.c_str(), "%[^=] = %[^;#]", key, value) == 2)
						data.set(section, key, value);
					else
						common::log::error("Error in parsing .ini file on line:\n" + line);
				}
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
