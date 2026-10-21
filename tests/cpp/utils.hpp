#pragma once

#include <list>
#include <optional>
#include <numeric>
#include <gtest/gtest.h>

#include "ioh/common/log.hpp"
#include "ioh/common/file.hpp"

#include <algorithm>
#include <cmath>
#include <regex>
#include <sstream>
#include <vector>
#include <string>

//! Relative tolerance for comparing computed values across platforms: the last digits differ
//! between compilers/architectures (e.g. fused multiply-add contraction on arm64)
inline double relative_tolerance(const double expected, const double rel = 1e-12)
{
    return rel * std::max(1.0, std::abs(expected));
}

// Template function to create a string representation of the contents of a vector
template<typename T>
inline std::string format_vector(const std::vector<T>& vec) {
  std::stringstream ss;
  ss << "[";
  for (size_t i = 0; i < vec.size(); ++i) {
    ss << vec[i];
    if (i != vec.size() - 1) {
      ss << ", ";
    }
  }
  ss << "]";
  return ss.str();
}

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

// Helper function to parse a string representation of a vector of vectors
inline std::vector<std::vector<int>> parse_vector_of_vectors(const std::string& vec_str) {
  std::vector<std::vector<int>> result;
  std::istringstream vec_stream(vec_str.substr(1, vec_str.size() - 2)); // Strip the outer brackets
  std::string subvec_str;

  while (std::getline(vec_stream, subvec_str, ']')) {
    std::vector<int> subvec;
    std::replace(subvec_str.begin(), subvec_str.end(), '[', ' ');
    std::replace(subvec_str.begin(), subvec_str.end(), ',', ' ');
    std::istringstream subvec_stream(subvec_str);
    int num;
    while (subvec_stream >> num) {
      subvec.push_back(num);
    }
    if (!subvec.empty()) {
      result.push_back(subvec);
    }
    vec_stream.get(); // Skip the comma after ']'
  }
  return result;
}

// Helper function to parse a string representation of a vector of integers
inline std::vector<int> parse_vector(const std::string& vec_str) {
  std::vector<int> result;
  std::istringstream vec_stream(vec_str.substr(1, vec_str.size() - 2)); // Strip the outer brackets
  std::string num_str;
  while (std::getline(vec_stream, num_str, ',')) {
    std::istringstream num_stream(num_str);
    int num;
    while (num_stream >> num) {
      result.push_back(num);
    }
  }
  return result;
}

// Helper function to create a string representation of a vector of integers
inline std::string vector_to_string(const std::vector<int>& vec) {
  std::ostringstream result;
  result << '[';
  for (size_t i = 0; i < vec.size(); ++i) {
    result << vec[i];
    if (i != vec.size() - 1) {
      result << ",";
    }
  }
  result << ']';
  return result.str();
}

inline std::string format_vector_of_vectors(const std::vector<std::vector<int>>& vec) {
  std::ostringstream oss;
  oss << "[";
  for (size_t i = 0; i < vec.size(); ++i) {
    if (i != 0) oss << ",";
    oss << "[";
    for (size_t j = 0; j < vec[i].size(); ++j) {
      if (j != 0) oss << ",";
      oss << vec[i][j];
    }
    oss << "]";
  }
  oss << "]";
  return oss.str();
}

inline std::string concatenate_vector(const std::vector<int>& v) {
  std::string result;
  for (int num : v) {
    result += std::to_string(num);
  }
  return result;
}

// Function to check if two vectors of vectors of integers are the same
inline bool are_vectors_of_vectors_equal(const std::vector<std::vector<int>>& vec1, const std::vector<std::vector<int>>& vec2) {
    if (vec1.size() != vec2.size()) {
        return false;
    }
    for (size_t i = 0; i < vec1.size(); ++i) {
        if (vec1[i] != vec2[i]) {
            return false;
        }
    }
    return true;
}

// Function to check if two vectors of integers are equal at each position
inline bool are_vectors_equal(const std::vector<int>& vec1, const std::vector<int>& vec2) {
  if (vec1.size() != vec2.size()) {
    return false;
  }
  for (size_t i = 0; i < vec1.size(); ++i) {
    if (vec1[i] != vec2[i]) {
      return false;
    }
  }
  return true;
}

//! A number found in text, with the value of one unit in its last printed digit
struct PrintedNumber
{
    double value;
    double last_digit;
};

//! Replace every unsigned decimal number in text by a placeholder that records its printed format,
//! collecting the numbers (to compare them with a tolerance). Integers (ids, dimensions, counts)
//! and signs stay in the text and are compared exactly.
inline std::string numbers_to_placeholders(const std::string& text, std::vector<PrintedNumber>& numbers){
    static const std::regex number(R"((\d+)(\.(\d*))?([eE]([-+]?\d+))?)");
    std::string skeleton;
    size_t last = 0;
    for (auto it = std::sregex_iterator(text.begin(), text.end(), number); it != std::sregex_iterator(); ++it)
    {
        const auto& m = *it;
        skeleton += text.substr(last, static_cast<size_t>(m.position()) - last);
        last = static_cast<size_t>(m.position() + m.length());
        if (!m[2].matched && !m[4].matched)
        {
            skeleton += m.str();
            continue;
        }
        const auto decimals = static_cast<int>(m[3].length());
        const auto exponent = m[5].matched ? std::stoi(m[5].str()) : 0;
        numbers.push_back({std::stod(m.str()), std::pow(10.0, exponent - decimals)});
        skeleton += "#" + std::to_string(decimals) + (m[4].matched ? "e" : "");
    }
    return skeleton + text.substr(last);
}

//! Compare a file with the expected text: layout, integers and the printed format exactly, decimal
//! numbers up to one unit in their last printed digit (computed values differ slightly between
//! platforms, see relative_tolerance)
inline void compare_file_with_string(const fs::path& path, const std::string& expected){
    const std::string got = ioh::common::file::as_string(path);
    std::vector<PrintedNumber> expected_numbers, got_numbers;
    const auto expected_skeleton = numbers_to_placeholders(expected, expected_numbers);
    const auto got_skeleton = numbers_to_placeholders(got, got_numbers);
    ASSERT_EQ(expected_skeleton, got_skeleton) << "EXPECTED:\n" << expected << "\nGOT:\n" << got;
    for (size_t i = 0; i < expected_numbers.size(); ++i)
    {
        const auto& e = expected_numbers[i];
        const auto tolerance = std::max(relative_tolerance(e.value), 1.0001 * e.last_digit);
        EXPECT_NEAR(e.value, got_numbers[i].value, tolerance) << "EXPECTED:\n" << expected << "\nGOT:\n" << got;
    }
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
