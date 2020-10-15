#include <filesystem>

#include <gtest/gtest.h>
#include "ioh.hpp"

namespace fs = std::filesystem;

TEST(experiment, config)
{
	fs::path config = fs::path("IOHexperimenter/example") / "conf.ini";
	fs::path root;
	bool found = false;
	for (const auto& e : fs::current_path())
	{
		root /= e;
		if (fs::exists(root / config))
		{
			config = root / config;
			found = true;
			break;
		}

		
	}
	
	if (!found)
	{
		std::cout << "Cannot find config file in: "
			<< fs::current_path() << std::endl;
		ASSERT_TRUE(true);
	}
		
	else
	{
		std::cout << "found conf file: " << config;

		ioh::experiment::configuration conf(config.generic_string());
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
}

	


