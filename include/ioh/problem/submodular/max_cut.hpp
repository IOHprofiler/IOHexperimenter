#pragma once

#include "graph_problem.hpp"

namespace ioh::problem
{
    namespace submodular
    {
        
        //! MaxCut Problem
        struct MaxCut final : GraphProblemType<MaxCut>
        {
            //! The registry entries (see registry_entries_of): one per graph in the definitions file, by default
            //! the example graphs; the dimension and the instance are fixed
            static std::vector<RegistryEntry<MaxCut>>
            registry_entries(const std::optional<fs::path> &definitions_file = std::nullopt);

            //! MaxCut problems start with problem ids starting from this id
            static inline int default_id = 2000;

            /**
             * @brief Construct a new MaxCut object. Suggested usage is via the factory.
             * If you want to create your own objects, please be sure to pass a correct graph instance.
             *
             * @param problem_id the id to the problem
             * @param graph the graph object on which to operate
             */
            MaxCut(const int problem_id, const int, const std::shared_ptr<graph::Graph> &graph) :
                GraphProblemType(problem_id, 1, fmt::format("MaxCut{}", problem_id), graph)
            {
            }
            
            //! Evaluate function
            double evaluate(const std::vector<int> &x) override
            {
                double result = 0;
                for (size_t source = 0; source < x.size(); source++)
                {
                    if (x[source])
                    {
                        for (const auto &[target, weight] : graph->adjacency_list[source])
                            result += weight * (x[target] == 0);
                    }
                    else if (graph->meta.digraph)
                    {
                        for (const auto &[target, weight] : graph->adjacency_list[source])
                            result -= weight * (x[target] == 1);
                    }
                }
                return result;
            }
        };
    } // namespace submodular

    inline std::vector<RegistryEntry<submodular::MaxCut>>
    submodular::MaxCut::registry_entries(const std::optional<fs::path> &definitions_file)
    {
        return definitions_file ? graph_entries(*definitions_file, "MaxCut") : builtin_entries("MaxCut");
    }

} // namespace ioh::problem