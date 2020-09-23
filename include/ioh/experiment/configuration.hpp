#pragma once
#include <algorithm>
#include "ioh/experiment/string.hpp"

namespace ioh
{
	namespace experiment
	{
		static int max_number_of_problem;
		static int max_dimension;

		class _dict
		{
		public:
			_dict()
			{
			};

			~_dict()
			{
			};
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
			configuration()
			{
			}

			int set_dict(_dict& dict, std::string section, std::string key, std::string value);

			std::string get_dict_String(_dict dict, std::string section, std::string key);

			std::vector<int> get_dict_int_vector(_dict dict, std::string section, std::string key, int _min, int _max);

			int get_dict_Int(_dict dict, std::string section, std::string key);

			bool get_dict_bool(_dict dict, std::string section, std::string key);

			linecontent add_Line(std::string input_line, std::string& section, std::string& key, std::string& value);

			_dict load(std::string filename);

			void readcfg(std::string filename);

			std::string get_suite_name();

			std::vector<int> get_problem_id();

			std::vector<int> get_instance_id();

			std::vector<int> get_dimension();

			std::string get_output_directory();

			std::string get_result_folder();

			std::string get_algorithm_info();

			std::string get_algorithm_name();

			bool get_complete_triggers();

			bool get_update_triggers();

			std::vector<int> get_base_evaluation_triggers();

			int get_number_target_triggers();

			int get_number_interval_triggers();
		};
	}
}
