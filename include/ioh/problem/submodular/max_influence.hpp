#pragma once
#include <queue>
#include <stdexcept>
#include "graph_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace submodular
        {
            class MaxInfluence final : public GraphProblem<MaxInfluence>
            {
            private:
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
                    int index = 0, count = 0;
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
                    cons_weight += sqrt(count) * graph->get_chance_cons_factor();
                    if (cons_weight > graph->get_cons_weight_limit()) // If the weight limit is exceeded (violating
                                                                      // constraint), return a penalized value
                        return graph->get_cons_weight_limit() - cons_weight;
                    double result = 0;
                    int repetitions = 100;
                    for (auto i = repetitions; i > 0; i--)
                    {
                        result += random_spread_count(seedIndex); // Simulate Independent Cascade Model
                    }
                    return seed_weight + result / repetitions; // Return average
                }

            public:
                MaxInfluence(const int instance = 1, const int n_variables = 1,
                             const std::string &instance_file = "example_list_maxinfluence") :
                    GraphProblem(3, // problem id, which will be overwritten when registering this class in all
                                    // pseudo-Boolean problems
                        instance, // the instance id
                        n_variables, // dimensions
                        "MaxInfluence", // problem name
                        false, // Using number of edges as dimension or not
                        instance_file)
                {
                    if (is_null())
                    {
                        std::cout << "Null MaxInfluence instance" << std::endl;
                        return;
                    }
                    /*if (graph->get_n_vertices() != graph->get_cons_weights_count())
                        throw std::invalid_argument("Number of constraint weights does not match number of vertices");*/
                    objective_.x = std::vector<int>(graph->get_n_vertices(), 1);
                    objective_.y = evaluate(objective_.x);
                }
            };
        } // namespace submodular
    } // namespace problem
} // namespace ioh