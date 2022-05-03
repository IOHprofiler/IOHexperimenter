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
            // Max Cut
            // Description: refer to Evolutionary Submodular Optimization website at https://cs.adelaide.edu.au/~optlog/CompetitionESO2022.php
            class MaxCut final : public GraphProblem<MaxCut>
            {
            protected:
                // [mandatory] The evaluate method is mandatory to implement
                double evaluate(const std::vector<int> &x) override
                {
                    double result = 0, cons_weight = 0;
                    int index = 0, count = 0;
                    for (auto &selected : x)
                    { // Iterate through 0-1 values
                        if (selected >= 1)
                        { // See if the vertex is selected
                            cons_weight += graph->get_cons_weight(index); // Add weight
                            count++;
                            int subindex = 0;
                            for (auto neighbor : graph->get_neighbors(index))
                            { // If the neighbors are not selected, add edge weights to the objective value
                                if (x[neighbor] == 0)
                                {
                                    result += graph->get_edge_weight(index, subindex);
                                }
                                subindex++;
                            }
                        }
                        else if (graph->get_is_digraph())
                        { // In digraph case, check unselected vertex as well
                            int subindex = 0;
                            for (auto neighbor : graph->get_neighbors(index))
                            { // If the neighbors are selected, substract edge weights from the objective value
                                if (x[neighbor] == 1)
                                {
                                    result -= graph->get_edge_weight(index, subindex);
                                }
                                subindex++;
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
                MaxCut(const int instance = 1, [[maybe_unused]] const int n_variable = 1,
                       const std::string &instance_file = Helper::instance_list_path.empty() ? "example_list_maxcut"
                           : Helper::instance_list_path) :
                    GraphProblem(instance + 2000000, // problem id, starting at 2000000
                        instance, // the instance id
                        read_instances_from_files(
                            instance - 1, false,
                            instance_file), // dimensions
                        "MaxCut" + std::to_string(instance), // problem name
                        false, // Using number of edges as dimension or not
                        instance_file)
                {
                    if (is_null())
                    {
                        IOH_DBG(warning, "Null MaxCut instance")
                        return;
                    }
                    objective_.x = std::vector<int>(graph->get_n_vertices(), 1);
                    objective_.y = evaluate(objective_.x);
                }
                // Constructor without n_variable, swap argument positions to avoid ambiguity
                MaxCut(const std::string &instance_file = Helper::instance_list_path, const int instance = 1) :
                    MaxCut(instance, 1, instance_file)
                {
                }
            };
        } // namespace submodular
    } // namespace problem
} // namespace ioh
