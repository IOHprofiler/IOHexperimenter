#pragma once
#include <ctime>

#include "ioh/common.hpp"
#include "ioh/suite/base.hpp"
#include "ioh/logger.hpp"
#include "configuration.hpp"

namespace ioh
{
	namespace experiment
	{
		template <typename ProblemType>
		class experimenter
		{
		public:
			typedef void algorithm_type(std::shared_ptr<ProblemType>, std::shared_ptr<logger::csv<ProblemType>>);

			experimenter() = delete;

			~experimenter()
			{
			}

			experimenter(std::string configFileName, algorithm_type* algorithm)
			{
				this->conf.readcfg(configFileName);

				configSuite = common::factory<suite::base<ProblemType>>::instance().create(
					conf.get_suite_name());
				if (configSuite == nullptr)
					common::log::error("Creating suite fails, please check your configuration");

				configSuite->set_suite_problem_id(conf.get_problem_id());
				configSuite->set_suite_instance_id(conf.get_instance_id());
				configSuite->set_suite_dimension(conf.get_dimension());
				configSuite->loadProblem();

				// std::shared_ptr<logger::csv<ProblemType>> logger(new logger::csv(
				// 	conf.get_output_directory(), conf.get_result_folder(), conf.get_algorithm_name(),
				// 	conf.get_algorithm_info()));
				auto logger = std::make_shared<logger::csv<ProblemType>>(
					conf.get_output_directory(), conf.get_result_folder(), conf.get_algorithm_name(),
					conf.get_algorithm_info());

				if (logger == nullptr)
					common::log::error("Creating logger fails, please check your configuration");

				logger->set_complete_flag(conf.get_complete_triggers());
				logger->set_interval(conf.get_number_interval_triggers());
				logger->set_time_points(conf.get_base_evaluation_triggers(), conf.get_number_target_triggers());
				logger->set_update_flag(conf.get_update_triggers());

				config_csv_logger = logger;
				config_csv_logger->open_index();

				this->algorithm = algorithm;
			}

			experimenter(std::shared_ptr<suite::base<ProblemType>> suite,
			             std::shared_ptr<logger::csv<ProblemType>> csv_logger,
			             algorithm_type* algorithm)
			{
				configSuite = suite;
				config_csv_logger = csv_logger;
				this->algorithm = algorithm;
			}

			void _run()
			{
				std::clock_t c_start_overall = std::clock();

				std::string info = "experiment\n";
				info += "Suite: " + this->configSuite->get_suite_name() + "\n";
				info += "Problem: " + vectorToString(this->configSuite->get_problem_id()) + "\n";
				info += "Instance: " + vectorToString(this->configSuite->get_instance_id()) + "\n";
				info += "Dimension: " + vectorToString(this->configSuite->get_dimension()) + "\n";
				print_info(info);

				this->config_csv_logger->track_suite(this->configSuite->get_suite_name());

				/// Problems are tested one by one until 'get_next_problem' returns NULL.
				while ((current_problem = configSuite->get_next_problem()) != nullptr)
				{
					std::clock_t c_start = std::clock();
					info = "f";
					info += std::to_string(current_problem->get_problem_id());
					info += "_d" + std::to_string(current_problem->get_number_of_variables());
					info += "_i" + std::to_string(current_problem->get_instance_id());
					print_info(info);


					this->config_csv_logger->track_problem(current_problem->get_problem_id(),
					                                       current_problem->get_number_of_variables(),
					                                       current_problem->get_instance_id(),
					                                       current_problem->get_problem_name(),
					                                       current_problem->get_optimization_type());

					algorithm(current_problem, this->config_csv_logger);

					print_info(".");

					int count = 1;
					while (independent_runs > count)
					{
						current_problem = configSuite->get_current_problem();
						this->config_csv_logger->track_problem(current_problem->get_problem_id(),
						                                       current_problem->get_number_of_variables(),
						                                       current_problem->get_instance_id(),
						                                       current_problem->get_problem_name(),
						                                       current_problem->get_optimization_type());
						algorithm(current_problem, this->config_csv_logger);
						++count;

						print_info(".");
					}

					std::clock_t c_end = std::clock();
					print_info("CPU Time" + std::to_string(1000.0 * (c_end - c_start) / CLOCKS_PER_SEC) + "ms\n");
				}

				std::clock_t c_end_overall = std::clock();
				print_info("Total CPU Time" + std::to_string(
					1000.0 * (c_end_overall - c_start_overall) / CLOCKS_PER_SEC) + "ms\n");
			}


			void _set_independent_runs(int n)
			{
				this->independent_runs = n;
			};

			void print_info(std::string info)
			{
				std::cout << info << std::flush;
			}

			std::string vectorToString(std::vector<int> v)
			{
				std::string s = "";
				if (v.size() == 0)
				{
					return s;
				}
				s = std::to_string(v[0]);
				for (int i = 1; i != v.size(); ++i)
				{
					s += " " + std::to_string(v[i]);
				}

				return s;
			}

		private:
			configuration conf;
			std::shared_ptr<suite::base<ProblemType>> configSuite;
			std::shared_ptr<ProblemType> current_problem;
			std::shared_ptr<logger::csv<ProblemType>> config_csv_logger;
			int independent_runs = 1;

			algorithm_type* algorithm;
		};
	}
}
