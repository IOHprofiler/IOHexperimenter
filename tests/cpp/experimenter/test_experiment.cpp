#include <algorithm>
#include <iostream>
#include <gtest/gtest.h>

#include "ioh.hpp"


fs::path find_config_file()
{
	auto config = fs::path("IOHexperimenter") / fs::path("example") / "conf.ini";
	fs::path root;
	for (const auto& e : fs::current_path())
	{
		root /= e;
		if (exists(root / config))
		{
			config = root / config;
			break;
		}
	}
	return config;
}


void bbob_random_search(std::shared_ptr<ioh::problem::bbob::bbob_base> problem,
                        std::shared_ptr<ioh::logger::Csv<ioh::problem::bbob::bbob_base>> logger)
{
	const auto n = problem->get_number_of_variables();
	std::vector<double> x(n);
	auto count = 0;
	auto best_y = -std::numeric_limits<double>::infinity();
	while (count++ <= 50)
	{
		ioh::common::Random::uniform(n, count, x);
		best_y = std::max(problem->evaluate(x), best_y);
		logger->do_log(problem->loggerCOCOInfo());

	}
}


void pbo_random_search(std::shared_ptr<ioh::problem::pbo::pbo_base> problem,
                       std::shared_ptr<ioh::logger::Csv<ioh::problem::pbo::pbo_base>> logger)
{
	const auto n = problem->get_number_of_variables();
	auto count = 0;
	auto best_y = -std::numeric_limits<double>::infinity();
	while (count++ <= 50)
	{
		const auto x = ioh::common::Random::integers(n, 0, 5);
		best_y = std::max(problem->evaluate(x), best_y);
		logger->do_log(problem->loggerCOCOInfo());
	}
}


int count_newlines(const std::string& s)
{
	return std::count_if(s.begin(), s.end(),
	                     [](const char& ch) { return ch == '\n'; });
}

TEST(experiment, config)
{
	auto config_file = find_config_file();
	const ioh::experiment::Configuration conf(config_file);
	ASSERT_EQ(conf.suite_name(), "PBO");
	ASSERT_EQ(conf.get_problem_ids(), std::vector<int>({ 1, 2, 3, 4, 5 }));
	ASSERT_EQ(conf.get_instance_id(), std::vector<int>({ 1 }));
	ASSERT_EQ(conf.dimensions(), std::vector<int>({ 16 }));
	ASSERT_EQ(conf.get_output_directory(), "./");
	ASSERT_EQ(conf.result_folder(), "Experiment");
	ASSERT_EQ(conf.get_algorithm_info(), "An_EA_algorithm");
	ASSERT_EQ(conf.algorithm_name(), "(1+1)_EA");
	ASSERT_EQ(conf.get_base_evaluation_triggers(), std::vector<int>({ 1 }));
	ASSERT_EQ(conf.update_triggers(), true);
	ASSERT_EQ(conf.get_complete_triggers(), false);
	ASSERT_EQ(conf.number_target_triggers(), 0);
	ASSERT_EQ(conf.get_number_interval_triggers(), 0);
}


TEST(experiment, pbo_from_config)
{
	using namespace ioh;
	auto config_file = find_config_file();
	experiment::Experimenter<ioh::problem::pbo::pbo_base> experiment(config_file, pbo_random_search);
	ASSERT_EQ(experiment.get_independent_runs(), 1);
	experiment.set_independent_runs(10);
	ASSERT_EQ(experiment.get_independent_runs(), 10);
	testing::internal::CaptureStdout();
	experiment.run();
	const auto output = testing::internal::GetCapturedStdout();
	ASSERT_EQ(count_newlines(output), 11);
	ASSERT_TRUE(fs::is_directory(experiment.get_logger()->experiment_directory()));
	ASSERT_TRUE(fs::is_directory(experiment.get_logger()->sub_directory()));
	ASSERT_TRUE(fs::is_regular_file(experiment.get_logger()->get_info_file_path()));
	ASSERT_TRUE(fs::is_regular_file(experiment.get_logger()->get_file_path(".dat")));
	experiment.get_logger()->clear_logger();
	try
	{
		fs::remove_all(experiment.get_logger()->experiment_directory()); // Cleanup	
	}
	catch (const std::exception& e)
	{
		common::log::info("Cannot remove directory: " 
				+ static_cast<std::string>(e.what()));
	}
}


TEST(experiment, bbob)
{
	using namespace ioh;
	std::vector<int> pbs = {1, 2};
	std::vector<int> ins = {1, 2};
	std::vector<int> dims = {2, 10};
	const auto suite = std::make_shared<suite::bbob>(pbs, ins, dims);
	const auto logger = std::make_shared<logger::Csv<problem::bbob::bbob_base>>();
	auto experiment = experiment::Experimenter<problem::bbob::bbob_base>(
		suite, logger, bbob_random_search, 10);

	
	ASSERT_EQ(experiment.get_independent_runs(), 10);
	testing::internal::CaptureStdout();
	experiment.run();
	const auto output = testing::internal::GetCapturedStdout();
	ASSERT_EQ(count_newlines(output), 14);
	ASSERT_TRUE(fs::is_directory(experiment.get_logger()->experiment_directory()));
	ASSERT_TRUE(fs::is_directory(experiment.get_logger()->sub_directory()));
	ASSERT_TRUE(fs::is_regular_file(experiment.get_logger()->get_info_file_path()));
	ASSERT_TRUE(fs::is_regular_file(experiment.get_logger()->get_file_path(".dat")));
	experiment.get_logger()->clear_logger();
	try
	{
		fs::remove_all(experiment.get_logger()->experiment_directory()); // Cleanup	
	}
	catch (const std::exception& e)
	{
		common::log::info("Cannot remove directory: "
			+ static_cast<std::string>(e.what()));
	}
}
