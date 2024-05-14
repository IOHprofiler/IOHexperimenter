#include "../utils.hpp"
#include "ioh/problem/dynamic_bin_val.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <vector>

int main(int, char **)
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

    for (int problem_id : problem_ids)
    {
        for (int instance : instances)
        {
            for (int n_variables : n_variables)
            {
                for (int number_of_timesteps : numbers_of_timesteps)
                {
                    for (int rep = 0; rep < repetitions; ++rep)
                    {

                        const auto &problem_factory =
                            ioh::problem::ProblemRegistry<ioh::problem::DynamicBinVal>::instance();
                        auto landscape = problem_factory.create(problem_id, instance, n_variables);

                        std::vector<int> x(n_variables);
                        std::uniform_int_distribution<int> distribution(0, 1);
                        std::generate(x.begin(), x.end(), [&]() { return distribution(generator); });

                        for (int i = 0; i < number_of_timesteps; ++i)
                        {
                            landscape->step();
                        }

                        double y = (*landscape)(x);

                        std::string x_str;
                        for (int bit : x)
                        {
                            x_str += std::to_string(bit);
                        }

                        // Output results directly during test execution
                        outfile << std::fixed << std::setprecision(5);
                        outfile << landscape->meta_data().problem_id << " " << instance << " " << number_of_timesteps
                                << " operator_call " << x_str << " " << y << std::endl;

                        if (landscape->meta_data().problem_id == 10'004)
                        {
                            for (int num_ranked_bitstrings : sequence_of_num_of_ranked_bitstrings)
                            {
                                std::vector<std::vector<int>> input_bitstrings;
                                for (int i = 0; i < num_ranked_bitstrings; ++i)
                                {
                                    std::vector<int> input_bitstring(n_variables);
                                    std::generate(input_bitstring.begin(), input_bitstring.end(),
                                                  [&]() { return distribution(generator); });
                                    input_bitstrings.push_back(input_bitstring);
                                }

                                std::vector<std::vector<int>> ranked_bitstrings =
                                    std::dynamic_pointer_cast<ioh::problem::dynamic_bin_val::Ranking>(landscape)->rank(
                                        input_bitstrings);
                                std::vector<int> ranking_indices =
                                    std::dynamic_pointer_cast<ioh::problem::dynamic_bin_val::Ranking>(landscape)
                                        ->rank_indices(input_bitstrings);

                                outfile << problem_id << " " << instance << " " << number_of_timesteps << " rank "
                                        << format_vector_of_vectors(input_bitstrings) << " "
                                        << format_vector_of_vectors(ranked_bitstrings) << std::endl;

                                outfile << problem_id << " " << instance << " " << number_of_timesteps
                                        << " rank_indices " << format_vector_of_vectors(input_bitstrings) << " "
                                        << vector_to_string(ranking_indices) << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }

    outfile.close();
}
