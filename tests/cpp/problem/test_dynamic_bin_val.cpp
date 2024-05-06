#include "../json.hpp"
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



TEST_F(BaseTest, test_dynamic_bin_val)
{
  std::ifstream infile;
  const auto file_path = ioh::common::file::utils::find_static_file("dynamic_bin_val.in");
  infile.open(file_path.c_str());

  const auto json_file_path = ioh::common::file::utils::find_static_file("dynamic_bin_val_rank.json");
  std::ifstream json_file(json_file_path);
  if (!json_file.is_open()) {
    FAIL() << "Error: Unable to open JSON file for reading.";
    return;
  }

  std::string line;
  while (std::getline(json_file, line)) {
    nlohmann::json scenario = nlohmann::json::parse(line);

    int problem_id = scenario["problem_id"];
    int instance = scenario["instance"];
    int number_of_timesteps = scenario["number_of_timesteps"];
    std::vector<std::vector<int>> input_bitstrings = scenario["bitstrings"];
    std::vector<std::vector<int>> ideal_ranked_bitstrings = scenario["rank"];

    ASSERT_EQ(problem_id, 10004) << "Problem ID is not 10004.";

    auto landscape = std::make_shared<ioh::problem::DynamicBinValRanking>(instance, input_bitstrings[0].size());

    for (int i = 0; i < number_of_timesteps; ++i) {
      landscape->step();
    }

    auto real_ranked_bitstrings = landscape->rank(input_bitstrings);

    EXPECT_TRUE(are_vectors_of_vectors_equal(ideal_ranked_bitstrings, real_ranked_bitstrings));
  }





  std::string s;
  while (getline(infile, s))
  {
    auto tmp = split(s, " ");
    if (tmp.empty()) { continue; }

    auto problem_id = stoi(tmp[0]);
    auto instance = stoi(tmp[1]);
    auto number_of_timesteps = stoi(tmp[2]);
    auto x = comma_separated_string_to_vector_int(tmp[3]);
    auto f = stod(tmp[4]);

    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::DynamicBinVal>::instance();
    int n_variables = x.size();
    auto landscape = problem_factory.create(problem_id, instance, n_variables);

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

#if GENERATE_TEST_DYNAMIC_BIN_VAL
TEST_F(BaseTest, generate_test_dynamic_bin_val)
{
  std::vector<int> problem_ids = {10001, 10002, 10003, 10004};
  std::vector<int> instances = {1, 2, 55, 667};
  std::vector<int> n_variables = {2, 3, 50};
  std::vector<int> numbers_of_timesteps = {0, 7, 22, 34};
  std::vector<int> sequence_of_num_of_ranked_bitstrings = {1, 10};
  const int repetitions = 3;
  const int generator_seed = 42;
  std::default_random_engine generator(generator_seed);

  // Specify the file path
  const auto static_root = ioh::common::file::utils::get_static_root();
  const auto file_path = static_root / "generated_dynamic_bin_val.in";
  const auto json_file_path = static_root / "generated_dynamic_bin_val_rank.json";

  // Open the file for writing
  std::ofstream outfile(file_path);
  std::ofstream json_file(json_file_path);

  for (int problem_id : problem_ids) {
    for (int instance : instances) {
      for (int n_variables : n_variables) {
        for (int number_of_timesteps : numbers_of_timesteps) {
          for (int rep = 0; rep < repetitions; ++rep) {
            switch (problem_id) {
              case 10001: {
                auto landscape = std::make_shared<ioh::problem::DynamicBinValUniform>(instance, n_variables);

                  std::vector<int> x(n_variables);
                  std::uniform_int_distribution<int> distribution(0, 1);
                  std::generate(x.begin(), x.end(), [&]() { return distribution(generator); });

                  for (int i = 0; i < number_of_timesteps; ++i) {
                    landscape->step();
                  }

                  double y = (*landscape)(x);

                  std::ostringstream oss;
                  std::copy(x.begin(), x.end(), std::ostream_iterator<int>(oss, ","));
                  std::string x_str = oss.str();
                  x_str.pop_back(); // Remove the trailing comma

                  // Output results directly during test execution
                  outfile << std::fixed << std::setprecision(5);
                  outfile << landscape->meta_data().problem_id << " " << instance << " " << number_of_timesteps
                          << " " << x_str << " " << y << std::endl;
              } break;

              case 10002: {
                auto landscape = std::make_shared<ioh::problem::DynamicBinValPowersOfTwo>(instance, n_variables);

                  std::vector<int> x(n_variables);
                  std::uniform_int_distribution<int> distribution(0, 1);
                  std::generate(x.begin(), x.end(), [&]() { return distribution(generator); });

                  for (int i = 0; i < number_of_timesteps; ++i) {
                    landscape->step();
                  }

                  double y = (*landscape)(x);

                  std::ostringstream oss;
                  std::copy(x.begin(), x.end(), std::ostream_iterator<int>(oss, ","));
                  std::string x_str = oss.str();
                  x_str.pop_back(); // Remove the trailing comma

                  // Output results directly during test execution
                  outfile << std::fixed << std::setprecision(5);
                  outfile << landscape->meta_data().problem_id << " " << instance << " " << number_of_timesteps
                          << " " << x_str << " " << y << std::endl;
              } break;

              case 10003: {
                auto landscape = std::make_shared<ioh::problem::DynamicBinValPareto>(instance, n_variables);

                  std::vector<int> x(n_variables);
                  std::uniform_int_distribution<int> distribution(0, 1);
                  std::generate(x.begin(), x.end(), [&]() { return distribution(generator); });

                  for (int i = 0; i < number_of_timesteps; ++i) {
                    landscape->step();
                  }

                  double y = (*landscape)(x);

                  std::ostringstream oss;
                  std::copy(x.begin(), x.end(), std::ostream_iterator<int>(oss, ","));
                  std::string x_str = oss.str();
                  x_str.pop_back(); // Remove the trailing comma

                  // Output results directly during test execution
                  outfile << std::fixed << std::setprecision(5);
                  outfile << landscape->meta_data().problem_id << " " << instance << " " << number_of_timesteps
                          << " " << x_str << " " << y << std::endl;
              } break;

              case 10004: {
                  auto landscape = std::make_shared<ioh::problem::DynamicBinValRanking>(instance, n_variables);

                    std::vector<int> x(n_variables);
                    std::uniform_int_distribution<int> distribution(0, 1);
                    std::generate(x.begin(), x.end(), [&]() { return distribution(generator); });

                    for (int i = 0; i < number_of_timesteps; ++i) {
                      landscape->step();
                    }

                    double y = (*landscape)(x);

                    std::ostringstream oss;
                    std::copy(x.begin(), x.end(), std::ostream_iterator<int>(oss, ","));
                    std::string x_str = oss.str();
                    x_str.pop_back(); // Remove the trailing comma

                    // Output results directly during test execution
                    outfile << std::fixed << std::setprecision(5);
                    outfile << landscape->meta_data().problem_id << " " << instance << " " << number_of_timesteps
                            << " " << x_str << " " << y << std::endl;

                for (int num_ranked_bitstrings : sequence_of_num_of_ranked_bitstrings) {
                  std::vector<std::vector<int>> input_bitstrings;
                  std::vector<std::vector<int>> ranked_bitstrings;
                  for (int i = 0; i < num_ranked_bitstrings; ++i) {
                    std::vector<int> input_bitstring(n_variables);
                    std::generate(input_bitstring.begin(), input_bitstring.end(), [&]() { return distribution(generator); });
                    input_bitstrings.push_back(input_bitstring);
                  }

                  ranked_bitstrings = landscape->rank(input_bitstrings);

                  nlohmann::json scenario = {
                    {"problem_id", problem_id},
                    {"instance", instance},
                    {"number_of_timesteps", number_of_timesteps},
                    {"bitstrings", input_bitstrings},
                    {"rank", ranked_bitstrings}
                  };

                  json_file << scenario.dump() + "\n";
                }

              } break;

              default:
                FAIL() << "Unknown problem ID: " << problem_id;
                continue;
            }
          }
        }
      }
    }
  }

  json_file.close();
  outfile.close();
}
#endif // GENERATE_TEST_DYNAMIC_BIN_VAL
