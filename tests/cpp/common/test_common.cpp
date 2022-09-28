#include  <array>
#include "../utils.hpp"

#include "ioh/common/optimization_type.hpp"
#include "ioh/common/log.hpp"
#include "ioh/common/factory.hpp"
#include "ioh/common/file.hpp"


TEST_F(BaseTest, common_test)
{
	using namespace ioh::common;
    
    auto min = FOptimizationType(OptimizationType::MIN);
    auto max = FOptimizationType(OptimizationType::MAX);

	EXPECT_TRUE(max(4, 2));
	EXPECT_FALSE(max(2, 2));

	EXPECT_TRUE(min(2, 5));
	EXPECT_FALSE(min(4, 2)); 
}

#ifdef NDEBUG
TEST_F(BaseTest, DISABLED_common_log)
#else
TEST_F(BaseTest, common_log)
#endif
{
    auto& ioh_dbg = clutchlog::logger();
    ioh_dbg.threshold(clutchlog::level::xdebug);
    ioh_dbg.file(".*");
    
    // @Johann: I don't know what this does, but the default constructor subtracts _strip_calls
    // from the depth. If I set it to std::numeric_limits<size_t>::max(), then no messages are logged.
    // I added the -5 (default value for _strip_calls) and then it again logs the expected message. 
    // Also, on windows this fails
    // ioh_dbg.depth(std::numeric_limits<size_t>::max() - 5);

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


TEST_F(BaseTest, common_file_parsing) {
    using namespace ioh::common;
    struct Row {
        std::array<std::string, 5> elements; 
        Row(const std::string& line): elements{} {
            std::stringstream ss(line);
            size_t i = 0;
            while (getline(ss, elements[i++], '|') && (i < elements.size()));
        }
    };

    auto file = file::utils::find_static_file("example_list_maxinfluence");
    EXPECT_TRUE(fs::is_regular_file(file));

    auto rows = file::as_text_vector<Row>(file);
    EXPECT_EQ(rows.size(), 24);
    EXPECT_EQ(rows.at(0).elements.at(0), "example_graphs/facebook_combined");
    
    auto integers = file::as_numeric_vector<int>(file::utils::find_static_file(rows.at(0).elements.at(3)));
    EXPECT_EQ(integers.size(), size_t{4040});
    EXPECT_EQ(integers.at(0), size_t{1});

    auto floats = file::as_numeric_vector<float>(file::utils::find_static_file(rows.at(0).elements.at(1)));
    EXPECT_EQ(floats.size(), size_t{176468});
    EXPECT_FLOAT_EQ(floats.at(0), 0.05882353f);
}   

