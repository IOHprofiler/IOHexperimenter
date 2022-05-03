// Author: Viet Anh Do

#pragma once
#include <queue>
#include <stdexcept>
#include <ioh/common/log.hpp>
#include "graph_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace submodular
        {
            // Max Influence
            // Description: refer to Evolutionary Submodular Optimization website at
            // https://cs.adelaide.edu.au/~optlog/CompetitionESO2022.php
            class MaxInfluence final : public GraphProblem<MaxInfluence>
            {
            private:
                int simulation_reps = 100;
                // Simulate Independent Cascade Model with seed and return weighted sum of activated nodes other than
                // seeds
                double random_spread_count(const std::vector<int> &seed)
                {
                    bool *is_activated = new bool[graph->get_n_vertices()]{0};
                    for (auto &selected : seed)
                    {
                        is_activated[selected] = true; // Activate seeded nodes
                    }
                    double total = 0;
                    std::queue<int> visits{
                        std::deque<int>(seed.begin(), seed.end())}; // Queue for BFS, simulate in chronological order
                    while (!visits.empty()) // Terminate when no more spreading
                    {
                        int currentIndex = visits.front();
                        visits.pop();
                        int subIndex = 0;
                        for (auto &neighbor : graph->get_neighbors(currentIndex)) // 2-way spread if undirected
                        { // Check if not already activated and spreading successfully
                            if (!is_activated[neighbor] &&
                                ioh::common::random::real() <= graph->get_edge_weight(currentIndex, subIndex))
                            {
                                is_activated[neighbor] = true; // Avoid redundant activation
                                visits.push(neighbor); // Cascading
                                total += graph->get_vertex_weight(neighbor); // Add weight
                            }
                            subIndex++;
                        }
                    }
                    return total;
                }

            protected:
                // [mandatory] The evaluate method is mandatory to implement
                double evaluate(const std::vector<int> &x) override
                {
                    double cons_weight = 0, seed_weight = 0;
                    int index = 0;
                    std::vector<int> seedIndex{};
                    for (auto &selected : x)
                    { // Iterate through 0-1 values
                        if (selected >= 1)
                        { // See if the vertex is selected
                            cons_weight += graph->get_cons_weight(index); // Add weight
                            seedIndex.push_back(index);
                            seed_weight += graph->get_vertex_weight(index);
                        }
                        index++; // Follow the current vertex by moving index, regardless of selection
                    }
                    cons_weight += sqrt(seedIndex.size()) * graph->get_chance_cons_factor();
                    if (cons_weight > graph->get_cons_weight_limit()) // If the weight limit is exceeded (violating
                                                                      // constraint), return a penalized value
                        return graph->get_cons_weight_limit() - cons_weight;
                    double result = 0;
                    for (auto i = simulation_reps; i > 0; i--)
                    {
                        result += random_spread_count(seedIndex); // Simulate Independent Cascade Model
                    }
                    return seed_weight + result / simulation_reps; // Return average
                }

            public:
                MaxInfluence(const int instance = 1, [[maybe_unused]] const int n_variables = 1,
                             const std::string &instance_file = Helper::instance_list_path.empty()
                                 ? "example_list_maxinfluence"
                                 : Helper::instance_list_path) :
                    GraphProblem(instance + 1000000, // problem id, starting at 1000000
                                 instance, // the instance id
                                 read_instances_from_files(instance - 1, false,
                                                           instance_file), // dimensions
                                 "MaxInfluence" + std::to_string(instance), // problem name
                                 false, // Using number of edges as dimension or not
                                 instance_file)
                {
                    if (is_null())
                    {
                        IOH_DBG(warning, "Null MaxInfluence instance")
                        return;
                    }
                    objective_.x = std::vector<int>(graph->get_n_vertices(), 1);
                    objective_.y = evaluate(objective_.x);
                }
                // Constructor without n_variable, swap argument positions to avoid ambiguity
                MaxInfluence(const std::string &instance_file = Helper::instance_list_path, const int instance = 1) :
                    MaxInfluence(instance, 1, instance_file)
                {
                }
                // Set number of times to repeat simulation, must be at least 1
                void set_simulation_reps(const int new_reps) { simulation_reps = std::max(new_reps, 1); }
            };
        } // namespace submodular
    } // namespace problem
} // namespace ioh
