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


void bbob_random_search(const std::shared_ptr<ioh::problem::Real> p)
{
	using namespace ioh::common;
	std::vector<double> x(p->meta_data().n_variables);
	auto count = 0;
	while (count++ < 10)
	{
		Random::uniform(p->meta_data().n_variables, Random::integer(), x);
		(*p)(x);
	}
}

void pbo_random_search(const std::shared_ptr<ioh::problem::Integer> p)
{
	using namespace ioh::common;
	auto count = 0;
	while (count++ < 10)
	{
		const auto x = Random::integers(p->meta_data().n_variables, 0, 5);
		(*p)(x);
	}
}


int count_newlines(const std::string& s)
{
	return static_cast<int>(std::count_if(s.begin(), s.end(),
	                     [](const char& ch) { return ch == '\n'; }));
}

TEST(experiment, config)
{
	auto config_file = find_config_file();
	const ioh::experiment::Configuration conf(config_file);
	ASSERT_EQ(conf.suite_name(), "PBO");
	ASSERT_EQ(conf.problem_ids(), std::vector<int>({ 1, 2, 3, 4, 5 }));
	ASSERT_EQ(conf.instances(), std::vector<int>({ 1 }));
	ASSERT_EQ(conf.dimensions(), std::vector<int>({ 16 }));
	ASSERT_EQ(conf.output_directory(), "./");
	ASSERT_EQ(conf.result_folder(), "Experiment");
	ASSERT_EQ(conf.algorithm_info(), "An_EA_algorithm");
	ASSERT_EQ(conf.algorithm_name(), "(1+1)_EA");
	ASSERT_EQ(conf.base_evaluation_triggers(), std::vector<int>({ 1 }));
	ASSERT_EQ(conf.update_triggers(), true);
	ASSERT_EQ(conf.complete_triggers(), false);
	ASSERT_EQ(conf.number_target_triggers(), 0);
	ASSERT_EQ(conf.number_interval_triggers(), 0);
}


TEST(experiment, pbo_from_config)
{
	using namespace ioh;
	auto config_file = find_config_file();
	experiment::Experimenter<problem::Integer> experiment(config_file, pbo_random_search);
	ASSERT_EQ(experiment.independent_runs(), 1);
	experiment.independent_runs(10);
	ASSERT_EQ(experiment.independent_runs(), 10);
	testing::internal::CaptureStdout();
	experiment.run();
	const auto output = testing::internal::GetCapturedStdout();
	ASSERT_GE(count_newlines(output), 5);
	// TODO: check that files are generated properly
	experiment.logger()->flush();
	try
	{
		fs::remove_all(dynamic_cast<logger::Default*>(&*experiment.logger())->experiment_folder().path()); // Cleanup	
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
	const auto suite = std::make_shared<suite::BBOB>(pbs, ins, dims);
	const auto logger = std::make_shared<logger::Default>(fs::current_path(),
		std::string("logger-experimenter"), "random-search", "10iterations", common::OptimizationType::Minimization, true);
	auto experiment = experiment::Experimenter<problem::Real>(
		suite, logger, bbob_random_search, 10);

	ASSERT_EQ(experiment.independent_runs(), 10);
	testing::internal::CaptureStdout();
	experiment.run();
	const auto output = testing::internal::GetCapturedStdout();
	ASSERT_GE(count_newlines(output), 8);
	// TODO: check that files are generated properly
	experiment.logger()->flush();
	try
	{
		fs::remove_all(dynamic_cast<logger::Default*>(&*experiment.logger())->experiment_folder().path()); // Cleanup	
	}
	catch (const std::exception& e)
	{
		common::log::info("Cannot remove directory: "
			+ static_cast<std::string>(e.what()));
	}
}
