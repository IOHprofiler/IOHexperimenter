// Author: Viet Anh Do

#pragma once
#include <stdexcept>
#include <ioh/common/log.hpp>
#include "graph_problem.hpp"

namespace ioh
{
    namespace problem
    {
        namespace submodular
        {
            // Max Coverage
            // Description: refer to Evolutionary Submodular Optimization website at https://cs.adelaide.edu.au/~optlog/CompetitionESO2022.php
            class MaxCoverage final : public GraphProblem<MaxCoverage>
            {
            protected:
                // [mandatory] The evaluate method is mandatory to implement
                double evaluate(const std::vector<int> &x) override
                {
                    bool *is_covered = new bool[graph->get_n_vertices()]{0};
                    double result = 0, cons_weight = 0;
                    int index = 0, count = 0;
                    for (auto &selected : x)
                    { // Iterate through 0-1 values
                        if (selected >= 1)
                        { // See if the vertex is selected
                            cons_weight += graph->get_cons_weight(index); // Add weight
                            count++;
                            if (!is_covered[index])
                            { // If the vertex is not covered, cover it and add its weight to the objective value
                                result += graph->get_vertex_weight(index);
                                is_covered[index] = true;
                            }
                            for (auto neighbor : graph->get_neighbors(index))
                            { // If the neighbors are not covered, cover them and add their weights to the objective
                              // value
                                if (!is_covered[neighbor])
                                {
                                    result += graph->get_vertex_weight(neighbor);
                                    is_covered[neighbor] = true;
                                }
                            }
                        }
                        index++; // Follow the current vertex by moving index, regardless of selection
                    }
                    cons_weight += sqrt(count) * graph->get_chance_cons_factor();
                    if (cons_weight > graph->get_cons_weight_limit()) // If the weight limit is exceeded (violating
                                                                      // constraint), return a penalized value
                        result = graph->get_cons_weight_limit() - cons_weight;
                    return result;
                }

            public:
                MaxCoverage(const int instance = 1, [[maybe_unused]] const int n_variable = 1,
                            const std::string &instance_file = Helper::instance_list_path.empty()
                                ? "example_list_maxcoverage"
                                : Helper::instance_list_path) :
                    GraphProblem(instance, // problem id, starting at 0
                        instance, // the instance id
                        read_instances_from_files(
                            instance - 1, false,
                            instance_file), // n_variables, which is configured by the given instance.
                        "MaxCoverage" + std::to_string(instance), // problem name
                        false, // Using number of edges as dimension or not
                        instance_file)
                {
                    if (is_null())
                    {
                        IOH_DBG(warning, "Null MaxCoverage instance")
                        return;
                    }
                    objective_.x = std::vector<int>(graph->get_n_vertices(), 1);
                    objective_.y = evaluate(objective_.x);
                }
                // Constructor without n_variable, swap argument positions to avoid ambiguity
                MaxCoverage(const std::string &instance_file = Helper::instance_list_path, const int instance = 1) :
                    MaxCoverage(instance, 1, instance_file)
                {
                }
            };
        } // namespace submodular
    } // namespace problem
} // namespace ioh
