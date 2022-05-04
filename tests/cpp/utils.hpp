#pragma once

#include <list>
#include <optional>
#include <numeric>
#include <clutchlog/clutchlog.h>
#include <gtest/gtest.h>

#include "ioh/common/file.hpp"

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

inline std::vector<int> string_to_vector_int(const std::string &s)
{
    std::vector<int> x;
    size_t i = 0;
    while (i != s.size())
    {
        x.push_back(s[i] - '0');
        i++;
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