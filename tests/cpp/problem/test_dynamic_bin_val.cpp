#include "../utils.hpp"
#include "ioh/problem/dynamic_bin_val.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <vector>

#define GENERATE_TEST_DYNAMIC_BIN_VAL false

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
      int n_variables = x.size();
      auto landscape = problem_factory.create(problem_id, instance, n_variables);

      if (problem_id == 10'003)
      {
        print_vector(std::dynamic_pointer_cast<ioh::problem::DynamicBinValPareto>(landscape)->weights);
      }

      for (int i = 0; i < number_of_timesteps; ++i) {
        landscape->step();
      }

      auto y = (*landscape)(x);

      if (f == 0.0) {
          EXPECT_NEAR(y, 0.0, 1.0 / pow(10, 6 - log(10)));
      } else {
          EXPECT_NEAR(y, f, 1.0 / pow(10, 6 - log(10)));
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

      int n_variables = input_bitstrings[0].size();
      auto landscape = std::make_shared<ioh::problem::DynamicBinValRanking>(instance, n_variables);

      for (int i = 0; i < number_of_timesteps; ++i) {
        landscape->step();
      }

      auto real_ranked_bitstrings = landscape->rank(input_bitstrings);

      EXPECT_TRUE(are_vectors_of_vectors_equal(ideal_ranked_bitstrings, real_ranked_bitstrings));
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

      int n_variables = input_bitstrings[0].size();
      auto landscape = std::make_shared<ioh::problem::DynamicBinValRanking>(instance, n_variables);

      for (int i = 0; i < number_of_timesteps; ++i) {
        landscape->step();
      }

      auto real_ranking_indices = landscape->rank_indices(input_bitstrings);

      EXPECT_TRUE(are_vectors_equal(ideal_ranking_indices, real_ranking_indices));
    }
  }
}

#if GENERATE_TEST_DYNAMIC_BIN_VAL
TEST_F(BaseTest, generate_test_dynamic_bin_val)
{
  std::vector<int> problem_ids = {10'001, 10'002, 10'003, 10'004};
  std::vector<int> instances = {1, 2, 55, 667};
  std::vector<int> n_variables = {2, 3, 50};
  std::vector<int> numbers_of_timesteps = {0, 7, 22, 34};
  std::vector<int> sequence_of_num_of_ranked_bitstrings = {1, 10};
  const int repetitions = 3;
  const int generator_seed = 42;
  std::default_random_engine generator(generator_seed);

  const auto static_root = ioh::common::file::utils::get_static_root();
  const auto file_path = static_root / "generated_dynamic_bin_val.in";

  // Open the file for writing
  std::ofstream outfile(file_path);

  for (int problem_id : problem_ids) {
    for (int instance : instances) {
      for (int n_variables : n_variables) {
        for (int number_of_timesteps : numbers_of_timesteps) {
          for (int rep = 0; rep < repetitions; ++rep) {

            const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::DynamicBinVal>::instance();
            auto landscape = problem_factory.create(problem_id, instance, n_variables);

            std::vector<int> x(n_variables);
            std::uniform_int_distribution<int> distribution(0, 1);
            std::generate(x.begin(), x.end(), [&]() { return distribution(generator); });

            for (int i = 0; i < number_of_timesteps; ++i) {
              landscape->step();
            }

            double y = (*landscape)(x);

            std::string x_str;
            for (int bit : x) {
              x_str += std::to_string(bit);
            }

            // Output results directly during test execution
            outfile << std::fixed << std::setprecision(5);
            outfile << landscape->meta_data().problem_id << " " << instance << " " << number_of_timesteps
                    << " operator_call " << x_str << " " << y << std::endl;

            if (landscape->meta_data().problem_id == 10'004) {
              for (int num_ranked_bitstrings : sequence_of_num_of_ranked_bitstrings) {
                std::vector<std::vector<int>> input_bitstrings;
                for (int i = 0; i < num_ranked_bitstrings; ++i) {
                  std::vector<int> input_bitstring(n_variables);
                  std::generate(input_bitstring.begin(), input_bitstring.end(), [&]() { return distribution(generator); });
                  input_bitstrings.push_back(input_bitstring);
                }

                std::vector<std::vector<int>> ranked_bitstrings = std::dynamic_pointer_cast<ioh::problem::DynamicBinValRanking>(landscape)->rank(input_bitstrings);
                std::vector<int> ranking_indices = std::dynamic_pointer_cast<ioh::problem::DynamicBinValRanking>(landscape)->rank_indices(input_bitstrings);

                outfile << problem_id << " " << instance << " " << number_of_timesteps << " rank "
                  << format_vector_of_vectors(input_bitstrings) << " " << format_vector_of_vectors(ranked_bitstrings) << std::endl;

                outfile << problem_id << " " << instance << " " << number_of_timesteps << " rank_indices "
                  << format_vector_of_vectors(input_bitstrings) << " " << vector_to_string(ranking_indices) << std::endl;
              }
            }
          }
        }
      }
    }
  }

  outfile.close();
}
#endif
