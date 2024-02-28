#include "../utils.hpp"

#include "ioh/problem/dynamic_bin_val.hpp"

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

#define LOG_FILE_NAME "test.log"

#define LOG(message)                                                           \
  do {                                                                         \
    std::ofstream debug_log(LOG_FILE_NAME, std::ios::app);                     \
    auto now = std::chrono::system_clock::now();                               \
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);          \
    debug_log << "["                                                           \
              << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") \
              << "] " << message << std::endl;                                 \
    debug_log.close();                                                         \
  } while (0)

int main() {
    const int population_size = 50;
    const int num_variables = 50;
    const int max_iterations = 10000;
    const double mutation_rate = 0.01;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    // Create an instance of DynamicBinValPareto
    ioh::problem::DynamicBinValPareto problem(1, num_variables);

    // Initialize population
    std::vector<std::vector<int>> population(population_size, std::vector<int>(num_variables));
    for (auto &individual : population) {
        for (auto &gene : individual) {
            gene = distribution(generator) < 0.5 ? 0 : 1;
        }
    }

    // Main GA loop
    for (int iteration = 0; iteration < max_iterations; ++iteration) {

      if (iteration < 2)
      {

        std::vector<int> best_individual = population[0];
        double best_fitness = problem(population[0]);
        std::cout << "Best Solution: ";
        for (int gene : best_individual) {
        std::cout << gene << " ";
        }
        std::cout << "\n iteration: " << iteration << "\n";
        std::cout << "\nBest Fitness: " << best_fitness << "\n";

        std::cout << "\n best_individual.state(): " << problem.state() << "\n";
      }

        // Evaluate population
        std::vector<double> fitness(population_size);
        for (int i = 0; i < population_size; ++i) {
            fitness[i] = problem(population[i]);
        }

        // Selection (tournament selection)
        std::vector<std::vector<int>> mating_pool;
        for (int i = 0; i < population_size; ++i) {
            int a = std::uniform_int_distribution<>(0, population_size - 1)(generator);
            int b = std::uniform_int_distribution<>(0, population_size - 1)(generator);
            mating_pool.push_back(fitness[a] > fitness[b] ? population[a] : population[b]);
        }

        // Crossover (uniform crossover)
        for (int i = 0; i < population_size; i += 2) {
            for (int j = 0; j < num_variables; ++j) {
                if (distribution(generator) < 0.5) {
                    std::swap(mating_pool[i][j], mating_pool[i+1][j]);
                }
            }
        }

        // Mutation
        for (auto &individual : mating_pool) {
            for (auto &gene : individual) {
                if (distribution(generator) < mutation_rate) {
                    gene = 1 - gene; // Flip the gene
                }
            }
        }

        // Replace the old population with the new population
        population = mating_pool;
    }

    // Find the best solution in the final population
    double best_fitness = problem(population[0]);
    std::vector<int> best_individual = population[0];
    for (int i = 1; i < population_size; ++i) {
        double current_fitness = problem(population[i]);
        if (current_fitness > best_fitness) {
            best_fitness = current_fitness;
            best_individual = population[i];
        }
    }

    // Output the best solution
    std::cout << "Best Solution: ";
    for (int gene : best_individual) {
        std::cout << gene << " ";
    }
    std::cout << "\nBest Fitness: " << best_fitness << "\n";

    std::cout << "\n best_individual.state(): " << problem.state() << "\n";
    std::cout << "\n best_individual.state().current_best_internal: " << problem.state().current_best_internal << "\n";
    std::cout << "\n best_individual.state().current_best: " << problem.state().current_best << "\n";
    std::cout << "\n best_individual.state().current_internal: " << problem.state().current_internal << "\n";
    std::cout << "\n best_individual.state().current: " << problem.state().current << "\n";

    return 0;
}
