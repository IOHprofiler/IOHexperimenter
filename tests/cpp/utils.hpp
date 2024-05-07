#pragma once

#include <list>
#include <optional>
#include <numeric>
#include <gtest/gtest.h>

#include "ioh/common/log.hpp"
#include "ioh/common/file.hpp"

// Function to print the contents of a vector of doubles
inline void print_vector(const std::vector<double>& weights) {
  std::cout << "Vector contents: [";
  for (size_t i = 0; i < weights.size(); ++i) {
    std::cout << weights[i];
    if (i != weights.size() - 1) {
      std::cout << ", ";
    }
  }
  std::cout << "]" << std::endl;
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
