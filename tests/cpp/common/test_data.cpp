#include "../utils.hpp"

#include <cstdlib>
#include <fstream>
#include <random>

#include "ioh/common/data.hpp"

namespace
{
    //! Sets (or, with an empty value, removes) an environment variable
    void set_env(const char *name, const std::string &value)
    {
#ifdef _WIN32
        _putenv_s(name, value.c_str());
#else
        if (value.empty())
            unsetenv(name);
        else
            setenv(name, value.c_str(), 1);
#endif
    }

    std::string get_env(const char *name)
    {
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996) // getenv
#endif
        const char *value = std::getenv(name);
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
        return value == nullptr ? "" : value;
    }
} // namespace

TEST_F(BaseTest, test_data_root)
{
    // without set_root or IOH_RESOURCES, the tests find the data in the source tree, the default of the build
    const auto resources = get_env("IOH_RESOURCES");
    set_env("IOH_RESOURCES", "");
    const auto default_root = ioh::data::root();
    EXPECT_TRUE(fs::is_regular_file(default_root / "example_list_maxcut"));
    EXPECT_EQ(ioh::data::file("example_list_maxcut"), default_root / "example_list_maxcut");

    const auto dir = fs::temp_directory_path() / ("ioh_test_data_root_" + std::to_string(std::random_device{}()));
    ASSERT_TRUE(fs::create_directories(dir));
    // restores the data directory and IOH_RESOURCES, and removes the files, also when the test fails
    struct Cleanup
    {
        fs::path dir;
        std::string resources;
        ~Cleanup()
        {
            ioh::data::set_root({});
            set_env("IOH_RESOURCES", resources);
            std::error_code ec;
            fs::remove_all(dir, ec);
        }
    } cleanup{dir, resources};
    std::ofstream(dir / "present") << "1";

    // IOH_RESOURCES comes before the default, and set_root before both
    set_env("IOH_RESOURCES", (dir / "missing").string());
    EXPECT_THROW((void)ioh::data::root(), ioh::data::data_error);
    set_env("IOH_RESOURCES", dir.string());
    EXPECT_EQ(ioh::data::root(), dir);
    ioh::data::set_root(default_root);
    EXPECT_EQ(ioh::data::root(), default_root);
    ioh::data::set_root(dir);
    EXPECT_EQ(ioh::data::file("present"), dir / "present");

    // a missing file, or a directory that does not exist, is an error that says where it looked
    try
    {
        (void)ioh::data::file("missing");
        ADD_FAILURE() << "no error for a missing file";
    }
    catch (const ioh::data::data_error &e)
    {
        const std::string message = e.what();
        EXPECT_NE(message.find("missing"), std::string::npos) << message;
        EXPECT_NE(message.find("ioh::data::set_root"), std::string::npos) << message;
    }
    ioh::data::set_root(dir / "missing");
    EXPECT_THROW((void)ioh::data::root(), ioh::data::data_error);

    // unset, the environment variable counts again
    ioh::data::set_root({});
    EXPECT_EQ(ioh::data::root(), dir);
}
