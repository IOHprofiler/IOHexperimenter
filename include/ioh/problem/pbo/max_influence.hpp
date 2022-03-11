#pragma once
#include <stdexcept>
#include <queue>
#include "graph_problem.hpp"
#include "ioh.hpp"

namespace ioh::problem::pbo
{
    class MaxInfluence final : public GraphProblem<MaxInfluence>
    {
    private:
        // Simulate Independent Cascade Model with seed and return weighted sum of activated nodes other than seeds
        double random_spread_count(const std::vector<int> &seed)
        {
            bool *is_activated = new bool[graph.get_n_vertices()]{0};
            for (const int &selected : seed)
            {
                is_activated[selected] = true;// Activate seeded nodes
            }
            double total = 0;
            std::queue<int> visits{std::deque<int>(seed.begin(), seed.end())}; // Queue for BFS, simulate in chronological order
            while (!visits.empty())// Terminate when no more spreading
            {
                int currentIndex = visits.front();
                visits.pop();
                int subIndex = 0;
                for (const int &neighbor : graph.get_neighbors(currentIndex))// 2-way spread if undirected
                {// Check if not already activated and spreading successfully
                    if (!is_activated[neighbor] &&
                        ioh::common::random::real() <= graph.get_edge_weight(currentIndex, subIndex))
                    {
                        is_activated[neighbor] = true;// Avoid redundant activation
                        visits.push(neighbor);// Cascading
                        total += graph.get_vertex_weight(neighbor);// Add weight
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
            for (const int &selected : x)
            { // Iterate through 0-1 values
                if (selected >= 1)
                { // See if the vertex is selected
                    cons_weight += graph.get_cons_weight(index); // Add weight
                    seedIndex.push_back(index);
                    seed_weight += graph.get_vertex_weight(index);
                }
                index++; // Follow the current vertex by moving index, regardless of selection
            }
            if (cons_weight > graph.get_cons_weight_limit()) // If the weight limit is exceeded (violating constraint),
                                                             // return a penalized value
                return graph.get_cons_weight_limit() - cons_weight;
            double result = 0;
            int repetitions = 100;
            for (auto i = repetitions; i > 0; i--)
            {
                result += random_spread_count(seedIndex);// Simulate Independent Cascade Model
            }
            return seed_weight + result / repetitions; // Return average
        }

    public:
        MaxInfluence(const int instance, const int n_variables = 1) :
            GraphProblem(
                103, // problem id, which will be overwritten when registering this class in all pseudo-Boolean problems
                instance, // the instance id
                get_dimensions_from_ids({instance - 1})[0], // dimensions
                "MaxInfluence" // problem name
            )
        {
            /*if (graph.get_n_vertices() != graph.get_cons_weights_count())
                throw std::invalid_argument("Number of constraint weights does not match number of vertices");*/
            objective_.x = std::vector<int>(graph.get_n_vertices(), 1);
            objective_.y = evaluate(objective_.x);
        }
    };
} // namespace ioh::problem::pbo