#pragma once

#include <queue>
#include "graph_problem.hpp"

namespace ioh::problem
{
    namespace submodular
    {
        //! MaxInfluence problem definition
        struct MaxInfluence final : GraphProblemType<MaxInfluence>
        {
            //! MaxInfluence problems start with problem ids starting from this id
            static inline int default_id = 2200;

            //! The number of repetitions to used for \ref random_spread_count
            int simulation_reps = 100;

            //! Vector of activations
            std::vector<uint8_t> is_activated;

            /**
             * @brief Construct a new MaxInfluence object. Suggested usage is via the factory.
             * If you want to create your own objects, please be sure to pass a correct graph instance.
             *
             * @param problem_id the id to the problem
             * @param graph the graph object on which to operate
             */
            MaxInfluence(const int problem_id, const int, const std::shared_ptr<graph::Graph> &graph) :
                GraphProblemType(problem_id, 1, fmt::format("MaxInfluence{}", problem_id), graph),
                is_activated(std::vector<uint8_t>(graph->dimension(), 0))
            {
                
            }
            /**
             * @brief Random process to compute the spread of x
             * 
             * @param x the candidate solutions
             * @return double the spread of x
             */
            double random_spread_count(const std::vector<int> &x)
            {
                double total = 0;

                std::queue<size_t> visits;

                for (size_t i = 0; i < x.size(); i++)
                {
                    is_activated[i] = static_cast<uint8_t>(x[i]);
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


            //! MaxInfluence evaluate function
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
    

    /**
     * @brief Template instantiation for pwt problems.
     * Ensures loading of PWT problems in the factory, using the correct files.
     * 
     * @param definitions_file optional file name
     * @return InstanceBasedProblem::Constructors<submodular::PackWhileTravel, int, int> a vector of contructor functions
     */
    template <>
    inline InstanceBasedProblem::Constructors<submodular::MaxInfluence, int, int>
    InstanceBasedProblem::load_instances<submodular::MaxInfluence>(const std::optional<fs::path> &definitions_file)
    {
        using namespace submodular;
        return GraphProblemType<MaxInfluence>::get_constructors(
            definitions_file.value_or(common::file::utils::find_static_file("example_list_maxinfluence")));
    }
} // namespace ioh::problem