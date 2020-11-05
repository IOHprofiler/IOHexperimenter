
#ifdef FSEXPERIMENTAL
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#include <gtest/gtest.h>
#include "ioh.hpp"
#include <iostream>

TEST(experiment, config)
{
	fs::path config = fs::path("IOHexperimenter") / fs::path("example") / "conf.ini";
	fs::path root;
	for (const auto& e : fs::current_path())
	{
		root /= e;
		if (fs::exists(root / config))
		{
			config = root / config;
			break;
		}
	}

	std::cout << "before" << std::endl;
	ioh::experiment::configuration conf(config.generic_string());
	std::cout << "before" << std::endl;
	ASSERT_EQ(conf.get_suite_name(), "PBO");
	ASSERT_EQ(conf.get_problem_id(), std::vector<int>({ 1, 2, 3, 4, 5 }));
	ASSERT_EQ(conf.get_instance_id(), std::vector<int>({ 1 }));
	ASSERT_EQ(conf.get_dimension(), std::vector<int>({ 16 }));
	ASSERT_EQ(conf.get_output_directory(), "./");
	ASSERT_EQ(conf.get_result_folder(), "Experiment");
	ASSERT_EQ(conf.get_algorithm_info(), "An_EA_algorithm");
	ASSERT_EQ(conf.get_algorithm_name(), "(1+1)_EA");
	ASSERT_EQ(conf.get_base_evaluation_triggers(), std::vector<int>({ 1 }));
	ASSERT_EQ(conf.get_update_triggers(), true);
	ASSERT_EQ(conf.get_complete_triggers(), false);
	ASSERT_EQ(conf.get_number_target_triggers(), 0);
	ASSERT_EQ(conf.get_number_interval_triggers(), 0);
}

	


