#pragma once
#include "graph_problem.hpp"
#include <stdexcept>

namespace ioh::problem::pbo
{
    class MaxVertexCover final : public GraphProblem<MaxVertexCover>
    {
    protected:
        // [mandatory] The evaluate method is mandatory to implement
        double evaluate(const std::vector<int> &x) override
        {
            bool *is_covered = new bool[graph.get_n_vertices()]{0};
            double result = 0, cons_weight = 0;
            int index = 0;
            for (const int &selected : x)
            { // Iterate through 0-1 values
                if (selected >= 1)
                { // See if the vertex is selected
                    cons_weight += graph.get_cons_weight(index); // Add weight
                    if (!is_covered[index])
                    { // If the vertex is not covered, cover it and add its weight to the objective value
                        result += graph.get_vertex_weight(index);
                        is_covered[index] = true;
                    }
                    for (int neighbor : graph.get_neighbors(index))
                    { // If the neighbors are not covered, cover them and add their weights to the objective value
                        if (!is_covered[neighbor])
                        {
                            result += graph.get_vertex_weight(neighbor);
                            is_covered[neighbor] = true;
                        }
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
        MaxVertexCover(const int instance, const int n_variables) :
            GraphProblem(
                101, // problem id, which will be overwritten when registering this class in all pseudo-Boolean problems
                instance, // the instance id
                get_dimensions_from_ids({instance - 1})[0], // dimensions
                "MaxVertexCover" // problem name
            )
        {
            if(graph.get_n_vertices() != graph.get_cons_weights_count()) 
                throw std::invalid_argument("Number of constraint weights does not match number of vertices");
            objective_.x = std::vector<int>(graph.get_n_vertices(), 1);
            objective_.y = evaluate(objective_.x);
        }
    };
}