#pragma once

#include <queue>
#include "graph_problem.hpp"

namespace ioh::problem
{
    namespace submodular
    {
        struct MaxInfluence final : GraphProblemType<MaxInfluence>
        {
            static inline int default_id = 2200;
            int simulation_reps = 100;
            std::vector<uint8_t> is_activated;

            /**
             * @brief Construct a new MaxInfluence object. Suggested usage is via the factory.
             * If you want to create your own objects, please be sure to pass a correct graph instance.
             *
             * @param problem_id the id to the problem
             * @param instance_id for instance based problems this is ignored
             * @param graph the graph object on which to operate
             */
            MaxInfluence(const int problem_id, const int, const std::shared_ptr<graph::Graph> &graph) :
                GraphProblemType(problem_id, 1, fmt::format("MaxInfluence{}", problem_id), graph),
                is_activated(std::vector<uint8_t>(graph->dimension(), 0))
            {
                
            }

            double random_spread_count(const std::vector<int> &x)
            {
                double total = 0;

                std::queue<size_t> visits;

                for (size_t i = 0; i < x.size(); i++)
                {
                    is_activated[i] = x[i];
                    if (x[i] != 0)
                        visits.push(i);
                }

                while (!visits.empty())
                {
                    size_t source = visits.front();
                    visits.pop();

                    for (const auto &[target, weight] : graph->adjacency_list[source])
                    {
                        const double r = common::random::real();
                        if (!is_activated[target] && r <= weight)
                        {
                            is_activated[target] = 1;
                            visits.push(target);
                            total += graph->vertex_weights[target];
                        }
                    }
                }
                return total;
            }

            double evaluate(const std::vector<int> &x) override
            {
                double result = 0.0;
                for (size_t source = 0; source < x.size(); source++)
                    result += graph->vertex_weights[source] * x[source];

                for (auto i = 0; i < simulation_reps; i++)
                    result += random_spread_count(x) / simulation_reps;
                return result;
            }
        };
    } // namespace submodular
    
    template <>
    inline InstanceBasedProblem::Constructors<submodular::MaxInfluence, int, int>
    InstanceBasedProblem::load_instances<submodular::MaxInfluence>(const std::optional<fs::path> &definitions_file)
    {
        using namespace submodular;
        return GraphProblemType<MaxInfluence>::get_constructors(
            definitions_file.value_or(common::file::utils::find_static_file("example_list_maxinfluence")));
    }
} // namespace ioh::problem