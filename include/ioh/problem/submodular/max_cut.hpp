#pragma once

#include "graph_problem.hpp"

namespace ioh::problem
{
    namespace submodular
    {
        struct MaxCut final : GraphProblemType<MaxCut>
        {
            static inline int default_id = 2000;

            /**
             * @brief Construct a new MaxCut object. Suggested usage is via the factory.
             * If you want to create your own objects, please be sure to pass a correct graph instance.
             *
             * @param problem_id the id to the problem
             * @param instance_id for instance based problems this is ignored
             * @param graph the graph object on which to operate
             */
            MaxCut(const int problem_id, const int, const std::shared_ptr<graph::Graph> &graph) :
                GraphProblemType(problem_id, 1, fmt::format("MaxCut{}", problem_id), graph)
            {
                objective_.x = std::vector<int>(graph->dimension(), 1);
                objective_.y = evaluate(objective_.x);
            }

            double evaluate(const std::vector<int> &x) override
            {
                double result = 0, constraint = 0;
                int count = 0;
                for (size_t source = 0; source < x.size(); source++)
                {
                    if (x[source])
                    {
                        constraint += graph->constraint_weights[source];
                        count++;
                        for (const auto &[target, weight] : graph->adjacency_list[source])
                            result += weight * (x[target] == 0);
                    }
                    else if (graph->meta.digraph)
                    {
                        for (const auto &[target, weight] : graph->adjacency_list[source])
                            result -= weight * (x[target] == 1);
                    }
                }
                constraint += sqrt(count) * graph->meta.chance_cons;
                if (constraint > graph->meta.constraint_limit)
                    result = graph->meta.constraint_limit - constraint;
                return result;
            }
        };
    } // namespace submodular


    template <>
    inline InstanceBasedProblem::Constructors<submodular::MaxCut, int, int>
    InstanceBasedProblem::load_instances<submodular::MaxCut>(const std::optional<fs::path> &definitions_file)
    {
        using namespace submodular;
        return GraphProblemType<MaxCut>::get_constructors(
            definitions_file.value_or(common::file::utils::find_static_file("example_list_maxcut")));
    }

} // namespace ioh::problem