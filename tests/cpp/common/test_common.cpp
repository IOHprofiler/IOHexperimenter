#include "../utils.hpp"

#include "ioh/common/optimization_type.hpp"
#include "ioh/common/log.hpp"
#include "ioh/common/factory.hpp"
#include "ioh/common/file.hpp"


TEST_F(BaseTest, common_test)
{
	using namespace ioh::common;
    
    auto min = FOptimizationType(OptimizationType::Minimization);
    auto max = FOptimizationType(OptimizationType::Maximization);

	EXPECT_TRUE(max(4, 2));
	EXPECT_FALSE(max(2, 2));

	EXPECT_TRUE(min(2, 5));
	EXPECT_FALSE(min(4, 2)); 
}

 
TEST_F(BaseTest, common_log)
{
    auto& ioh_dbg = clutchlog::logger();
    ioh_dbg.threshold(clutchlog::level::xdebug);
    ioh_dbg.file(".*");
    
    // @Johann Jacob: I dont know what this does, but the default constructor subtracts _strip_calls
    // from the depth. If I set it to std::numeric_limits<size_t>::max(), then no messages are logged.
    // I added the -5 (default value for _strip_calls) and then it logs. 
    ioh_dbg.depth(std::numeric_limits<size_t>::max() - 5);

	testing::internal::CaptureStderr();
	ioh_dbg.format("{msg}");
	IOH_DBG(info, "Hello")
	auto output = testing::internal::GetCapturedStderr();
    EXPECT_EQ(output, "Hello\x1B[0m"); // Hello + color reset ANSI code
}

TEST_F(BaseTest, common_typenames)
{
    EXPECT_EQ(ioh::common::class_name<BaseTest>(), "BaseTest");
}


TEST_F(BaseTest, common_unique_folder) {
    using namespace ioh::common::file;
    const std::string f_name = "TEST_FOLDER";
    remove_all(fs::current_path() / f_name);

    const auto f = UniqueFolder(f_name);
    EXPECT_TRUE(fs::exists(f.path()));
    EXPECT_EQ(f.name(), f_name);

    const auto f2 = UniqueFolder(f_name);
    EXPECT_TRUE(fs::exists(f2.path()));
    EXPECT_EQ(f2.name(), "TEST_FOLDER-1");

    f.remove();
    EXPECT_FALSE(fs::exists(f.path()));

    f2.remove();
    EXPECT_FALSE(fs::exists(f2.path()));
}
