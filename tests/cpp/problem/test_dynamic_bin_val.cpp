#include "../utils.hpp"
#include "ioh/problem/dynamic_bin_val.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <vector>

TEST_F(BaseTest, test_cec2022)
{
  std::ifstream infile;
  const auto file_path = ioh::common::file::utils::find_static_file("dynamic_bin_val.in");
  infile.open(file_path.c_str());

  std::string s;
  while (getline(infile, s))
  {
    auto tmp = split(s, " ");
    if (tmp.empty()) { continue; }

    auto problem_id = stoi(tmp[0]);
    auto instance = stoi(tmp[1]);
    auto number_of_timesteps = stoi(tmp[2]);
    auto x = string_to_vector_int(tmp[3]);
    auto f = stod(tmp[4]);

    std::shared_ptr<ioh::problem::IntegerSingleObjective> landscape;

    switch (problem_id) {
      case 10001:
        landscape = std::make_shared<ioh::problem::DynamicBinValUniform>(instance, x.size());

        for (int i = 0; i < number_of_timesteps; ++i) {
          std::dynamic_pointer_cast<ioh::problem::DynamicBinValUniform>(landscape)->step();
        }

        break;
      case 10002:
        landscape = std::make_shared<ioh::problem::DynamicBinValPowersOfTwo>(instance, x.size());

        for (int i = 0; i < number_of_timesteps; ++i) {
          std::dynamic_pointer_cast<ioh::problem::DynamicBinValPowersOfTwo>(landscape)->step();
        }

        break;
      case 10003:
        landscape = std::make_shared<ioh::problem::DynamicBinValPareto>(instance, x.size());

        for (int i = 0; i < number_of_timesteps; ++i) {
          std::dynamic_pointer_cast<ioh::problem::DynamicBinValPareto>(landscape)->step();
        }

        break;
      case 10004:
        landscape = std::make_shared<ioh::problem::DynamicBinValRanking>(instance, x.size());

        for (int i = 0; i < number_of_timesteps; ++i) {
          std::dynamic_pointer_cast<ioh::problem::DynamicBinValRanking>(landscape)->step();
        }

        break;
      default:
        std::cerr << "Unknown function ID: " << problem_id << std::endl;
        continue;
    }

    auto y = (*landscape)(x);

    if (f == 0.0) {
        EXPECT_NEAR(y, 0.0, 1.0 / pow(10, 6 - log(10)));
    } else {
        EXPECT_NEAR(y, f, 1.0 / pow(10, 6 - log(10)));
    }

  }
}


TEST_F(BaseTest, DynamicBinValRepetitionsTest)
{
  std::vector<int> problem_ids = {10001, 10002, 10003};
  std::vector<int> instances = {1};
  std::vector<int> n_variables = {2, 3, 50, 250};
  std::vector<int> numbers_of_timesteps = {0, 7, 22, 34, 555};
  const int repetitions = 1;
  const int generator_seed = 42;
  std::default_random_engine generator(generator_seed);

  // Specify the file path
  const auto static_root = ioh::common::file::utils::get_static_root();
  const auto file_path = static_root / "generated_dynamic_bin_val.in";

  // Open the file for writing
  std::ofstream outfile(file_path);

  for (int problem_id : problem_ids) {
    for (int instance : instances) {
      for (int n_variables : n_variables) {
        for (int number_of_timesteps : numbers_of_timesteps) {

          switch (problem_id) {
            case 10001: {
              auto landscape = std::make_shared<ioh::problem::DynamicBinValUniform>(instance, n_variables);

              for (int rep = 0; rep < repetitions; ++rep) {
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
              }
            } break;

            case 10002: {
              auto landscape = std::make_shared<ioh::problem::DynamicBinValPowersOfTwo>(instance, n_variables);

              for (int rep = 0; rep < repetitions; ++rep) {
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
              }
            } break;

            case 10003: {
              auto landscape = std::make_shared<ioh::problem::DynamicBinValPareto>(instance, n_variables);

              for (int rep = 0; rep < repetitions; ++rep) {
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
              }
            } break;

            default:
              std::cerr << "Unknown problem ID: " << problem_id << std::endl;
              continue;
          }
        }
      }
    }
  }

  // Close the file after writing
  outfile.close();
}
