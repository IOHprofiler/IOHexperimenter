#pragma once
#include "graph_problem.hpp"
#include <stdexcept>

namespace ioh::problem::pbo
{
    class MaxCut final : public GraphProblem<MaxCut>
    {
    protected:
        // [mandatory] The evaluate method is mandatory to implement
        double evaluate(const std::vector<int> &x) override
        {
            double result = 0, cons_weight = 0;
            int index = 0;
            for (const int &selected : x)
            { // Iterate through 0-1 values
                if (selected >= 1)
                { // See if the vertex is selected
                    cons_weight += graph.get_cons_weight(index); // Add weight
                    int subindex = 0;
                    for (int neighbor : graph.get_neighbors(index))
                    { // If the neighbors are not selected, add edge weights to the objective value
                        if (x[neighbor] == 0)
                        {
                            result += graph.get_edge_weight(index,subindex);
                        }
                        subindex++;
                    }
                }
                else if (graph.get_is_digraph())
                { // In digraph case, check unselected vertex as well
                    int subindex = 0;
                    for (int neighbor : graph.get_neighbors(index))
                    { // If the neighbors are selected, substract edge weights from the objective value
                        if (x[neighbor] == 1)
                        {
                            result -= graph.get_edge_weight(index,subindex);
                        }
                        subindex++;
                    }
                }
                index++; // Follow the current vertex by moving index, regardless of selection
            }
            if (cons_weight > graph.get_cons_weight_limit()) // If the weight limit is exceeded (violating constraint), return
                                                       // a penalized value
                result = graph.get_cons_weight_limit() - cons_weight;
            return result;
        }

    public:
        MaxCut(const int instance, const int n_variables) :
            GraphProblem(
                102, // problem id, which will be overwritten when registering this class in all pseudo-Boolean problems
                instance, // the instance id
                get_dimensions_from_ids({instance - 1})[0], // dimensions
                "MaxCut" // problem name
            )
        {
            if (graph.get_n_vertices() != graph.get_cons_weights_count())
                throw std::invalid_argument("Number of constraint weights does not match number of vertices");
            objective_.x = std::vector<int>(graph.get_n_vertices(), 1);
            objective_.y = evaluate(objective_.x);
        }
    };
} // namespace ioh::problem::pbo