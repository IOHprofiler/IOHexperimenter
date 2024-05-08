#include "../utils.hpp"
#include "ioh/problem/dynamic_bin_val.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <vector>

// Helper function to check if the operation name is valid
bool is_valid_operation(const std::string& operation_name) {
  return operation_name == "operator_call" || operation_name == "rank" || operation_name == "rank_indices";
}

TEST_F(BaseTest, test_dynamic_bin_val_operator_call)
{
  std::ifstream infile;
  const auto file_path = ioh::common::file::utils::find_static_file("dynamic_bin_val.in");
  infile.open(file_path.c_str());

  std::string s;
  while (std::getline(infile, s))
  {
    auto tmp = split(s, " ");
    if (tmp.empty()) { continue; }

    auto problem_id = stoi(tmp[0]);
    auto instance = stoi(tmp[1]);
    auto number_of_timesteps = stoi(tmp[2]);

    auto operation_name = tmp[3];
    ASSERT_TRUE(is_valid_operation(operation_name)) << "Invalid operation name: " << operation_name;
    if (operation_name == "operator_call")
    {
      auto x = string_to_vector_int(tmp[4]);
      auto f = stod(tmp[5]);

      const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::DynamicBinVal>::instance();
      int n_variables = static_cast<int>(x.size());
      auto landscape = problem_factory.create(problem_id, instance, n_variables);

      for (int i = 0; i < number_of_timesteps; ++i) {
        landscape->step();
      }

      auto y = (*landscape)(x);

      if (problem_id == 10'002)
      {
        std::cout << format_vector(std::dynamic_pointer_cast<ioh::problem::DynamicBinValPowersOfTwo>(landscape)->weights) << std::endl;
      }

      if (f == 0.0) {
        EXPECT_NEAR(y, 0.0, 1.0 / pow(10, 6 - log(10)))
          << problem_id << " " << instance << " " << number_of_timesteps << " "
          << operation_name << " " << concatenate_vector(x) << " " << f;
      } else {
        EXPECT_NEAR(y, f, 1.0 / pow(10, 6 - log(10)))
          << problem_id << " " << instance << " " << number_of_timesteps << " "
          << operation_name << " " << concatenate_vector(x) << " " << f;
      }
    }
  }
}

TEST_F(BaseTest, test_dynamic_bin_val_rank)
{
  std::ifstream infile;
  const auto file_path = ioh::common::file::utils::find_static_file("dynamic_bin_val.in");
  infile.open(file_path.c_str());

  std::string s;
  while (std::getline(infile, s))
  {
    auto tmp = split(s, " ");
    if (tmp.empty()) { continue; }

    auto problem_id = stoi(tmp[0]);
    auto instance = stoi(tmp[1]);
    auto number_of_timesteps = stoi(tmp[2]);

    auto operation_name = tmp[3];
    ASSERT_TRUE(is_valid_operation(operation_name)) << "Invalid operation name: " << operation_name;
    if (operation_name == "rank")
    {
      auto bitstrings_str = tmp[4];
      auto ranks_str = tmp[5];

      // Convert string representations of vectors of vectors into actual data structures
      std::vector<std::vector<int>> input_bitstrings = parse_vector_of_vectors(bitstrings_str);
      std::vector<std::vector<int>> ideal_ranked_bitstrings = parse_vector_of_vectors(ranks_str);

      ASSERT_EQ(problem_id, 10'004) << "Problem ID is not 10'004.";

      int n_variables = static_cast<int>(input_bitstrings[0].size());
      auto landscape = std::make_shared<ioh::problem::DynamicBinValRanking>(instance, n_variables);

      for (int i = 0; i < number_of_timesteps; ++i) {
        landscape->step();
      }

      auto real_ranked_bitstrings = landscape->rank(input_bitstrings);

      EXPECT_TRUE(are_vectors_of_vectors_equal(ideal_ranked_bitstrings, real_ranked_bitstrings))
          << "Expected and actual ranked bitstrings do not match. Expected: "
          << format_vector_of_vectors(ideal_ranked_bitstrings)
          << ", Actual: " << format_vector_of_vectors(real_ranked_bitstrings)
          << ", comparison ordering: " << format_vector(landscape->comparison_ordering);
    }
  }
}

TEST_F(BaseTest, test_dynamic_bin_val_rank_indices)
{
  std::ifstream infile;
  const auto file_path = ioh::common::file::utils::find_static_file("dynamic_bin_val.in");
  infile.open(file_path.c_str());

  std::string s;
  while (std::getline(infile, s))
  {
    auto tmp = split(s, " ");
    if (tmp.empty()) { continue; }

    auto problem_id = stoi(tmp[0]);
    auto instance = stoi(tmp[1]);
    auto number_of_timesteps = stoi(tmp[2]);

    auto operation_name = tmp[3];
    ASSERT_TRUE(is_valid_operation(operation_name)) << "Invalid operation name: " << operation_name;
    if (operation_name == "rank_indices")
    {
      auto bitstrings_str = tmp[4];
      auto ranks_str = tmp[5];

      // Convert string representations of vectors of vectors into actual data structures
      std::vector<std::vector<int>> input_bitstrings = parse_vector_of_vectors(bitstrings_str);
      std::vector<int> ideal_ranking_indices = parse_vector(ranks_str);

      ASSERT_EQ(problem_id, 10'004) << "Problem ID is not 10'004.";

      int n_variables = static_cast<int>(input_bitstrings[0].size());
      auto landscape = std::make_shared<ioh::problem::DynamicBinValRanking>(instance, n_variables);

      for (int i = 0; i < number_of_timesteps; ++i) {
        landscape->step();
      }

      auto real_ranking_indices = landscape->rank_indices(input_bitstrings);

      EXPECT_TRUE(are_vectors_equal(ideal_ranking_indices, real_ranking_indices))
        << "Expected and actual ranked bitstrings do not match. Expected: "
        << vector_to_string(ideal_ranking_indices)
        << ", Actual: " << vector_to_string(real_ranking_indices);
    }
  }
}
