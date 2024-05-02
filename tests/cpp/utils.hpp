#pragma once

#include <list>
#include <optional>
#include <numeric>
#include <gtest/gtest.h>

#include "ioh/common/log.hpp"
#include "ioh/common/file.hpp"


inline void expect_vector_eq(const std::vector<int> &x, const std::vector<int> &y)
{
    ASSERT_EQ(x.size(), y.size()) << "Vectors x and y are of unequal length";

    for (size_t i = 0; i < x.size(); ++i)
        EXPECT_EQ(x[i], y[i]) << "Vectors x and y differ at index " << i;
}

inline void expect_vector_eq(const std::vector<double> &x, const std::vector<double> &y, const double eta = 1e-4)
{
    ASSERT_EQ(x.size(), y.size()) << "Vectors x and y are of unequal length";

    for (size_t i = 0; i < x.size(); ++i)
        EXPECT_NEAR(x[i], y[i], eta) << "Vectors x and y differ at index " << i;
}

inline std::vector<std::string> split(const std::string &str, const std::string &pattern)
{
    std::vector<std::string> res;
    if (str.empty())
        return res;

    auto search_string = str + pattern;
    auto pos = search_string.find(pattern);

    while (pos != search_string.npos)
    {
        res.push_back(search_string.substr(0, pos));
        search_string = search_string.substr(pos + 1, search_string.size());
        pos = search_string.find(pattern);
    }
    return res;
}

inline std::vector<double> string_to_vector_double(const std::string &s)
{
    std::vector<double> x;
    size_t i = 0;

    auto tmp = split(s, ",");
    while (i != tmp.size())
    {
        x.push_back(stod(tmp[i]));
        i++;
    }
    return x;
}

inline std::vector<int> string_to_vector_int(const std::string &s) {
    std::vector<int> x;
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, ',')) {
        if (!item.empty()) {
            x.push_back(stoi(item));
        }
    }
    return x;
}

inline void compare_file_with_string(const fs::path& path, const std::string& expected){
    const std::string got = ioh::common::file::as_string(path);
    EXPECT_EQ(0, got.compare(expected)) << "EXPECTED:\n" << expected << "\nGOT:\n" << got;
}

class BaseTest: public ::testing::Test
{
public:
    inline static clutchlog::level log_level_ = clutchlog::level::warning;
    inline static std::optional<int> log_depth_ = std::nullopt;
    inline static std::optional<std::string> log_file_ = std::nullopt;
protected:
    void SetUp() override;    
};
