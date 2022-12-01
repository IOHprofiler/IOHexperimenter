#include "../utils.hpp"

#include "../example/problem_example.hpp"
#include "../example/suite_example.hpp"
#include "../example/logger_example.hpp"
#include "../example/experiment_example.hpp"
#include "../example/add_new_problem.hpp"
#include "../example/example_submodular_instances.hpp"

TEST_F(BaseTest, show_registered_objects) { 
	testing::internal::CaptureStdout();
	show_registered_objects(); 
	std::string output = testing::internal::GetCapturedStdout();
}

TEST_F(BaseTest, problem_example) { 
	testing::internal::CaptureStdout();
    problem_example();
	std::string output = testing::internal::GetCapturedStdout();
}
TEST_F(BaseTest, suite_example) { 
	testing::internal::CaptureStdout();
    suite_example();
	std::string output = testing::internal::GetCapturedStdout();
}
TEST_F(BaseTest, experiment_example) { 
	testing::internal::CaptureStdout();
    experiment_example();
	std::string output = testing::internal::GetCapturedStdout();
}
TEST_F(BaseTest, logger_example) { 
	testing::internal::CaptureStdout();
    logger_example();
	std::string output = testing::internal::GetCapturedStdout();
}
TEST_F(BaseTest, add_problem_example) { 
	testing::internal::CaptureStdout();
    add_problem_example();
	std::string output = testing::internal::GetCapturedStdout();
}
TEST_F(BaseTest, DISABLED_example_submodular_suite_with_logging) { 
	testing::internal::CaptureStdout();
    example_submodular_suite_with_logging();
	std::string output = testing::internal::GetCapturedStdout();
}
