/// Test suite for the sample module.
///
/// Link all test files with the `gtest_main` library to create a command-line 
/// test runner.
///
#include <gtest/gtest.h>
#include "ioh.hpp"


TEST(common, test)
{
	using namespace ioh::common;
	ASSERT_TRUE(compare_objectives(4, 2, optimization_type::maximization));
	ASSERT_FALSE(compare_objectives(2, 2, optimization_type::maximization));

	ASSERT_TRUE(compare_objectives(2, 5, optimization_type::minimization));
	ASSERT_FALSE(compare_objectives(4, 2, optimization_type::minimization));
}


TEST(common, log)
{
	using namespace ioh::common::log;
	testing::internal::CaptureStdout();
	info("Hello");
	std::string output = testing::internal::GetCapturedStdout();
	ASSERT_EQ(output, "IOH_LOG_INFO : Hello\n");
	testing::internal::CaptureStdout();
	warning("Warning");
	output = testing::internal::GetCapturedStdout();
	ASSERT_EQ(output, "IOH_WARNING_INFO : Warning\n");
}


TEST(common, class_generator)
{
	using namespace ioh::common;
	// this stuff is too vague
	// std::cout << genericGenerator<int>::get() << std::endl;
}

TEST(common, random)
{
	using namespace ioh;
	common::random r(1);
	ASSERT_DOUBLE_EQ(r.normal_rand(), -5.8762480600075353);
	ASSERT_DOUBLE_EQ(r.uniform_rand(), 0.86453751611163321);
}
