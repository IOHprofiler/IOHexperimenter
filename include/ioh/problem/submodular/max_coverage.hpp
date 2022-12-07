#pragma once

#include "graph_problem.hpp"

namespace ioh::problem
{
    namespace submodular
    {
        //! Max Coverage problem definition
        struct MaxCoverage final : GraphProblemType<MaxCoverage>
        {
            //! MaxCoverage problems start with problem ids starting from this id
            static inline int default_id = 2100;

            //! The vector of convered points
            std::vector<uint8_t> is_covered;
            /**
             * @brief Construct a new MaxCoverage object. Suggested usage is via the factory.
             * If you want to create your own objects, please be sure to pass a correct graph instance.
             *
             * @param problem_id the id to the problem
             * @param graph the graph object on which to operate
             */
            MaxCoverage(const int problem_id, const int, const std::shared_ptr<graph::Graph> &graph) :
                GraphProblemType(problem_id, 1, fmt::format("MaxCoverage{}", problem_id), graph),
                is_covered(std::vector<uint8_t>(graph->dimension(), 0))
            {
            }   

            //! Evaluate function for MaxCoverage
            double evaluate(const std::vector<int> &x) override
            {
                std::fill(is_covered.begin(), is_covered.end(), 0);

                double result = 0;
                for (size_t source = 0; source < x.size(); source++)
                {
                    if (x[source])
                    {
                        if (!is_covered[source])
                        {
                            result += graph->vertex_weights[source];
                            is_covered[source] = 1;
                        }
                        for (const auto &[target, _] : graph->adjacency_list[source])
                        {
                            if (!is_covered[target])
                            {
                                result += graph->vertex_weights[target];
                                is_covered[target] = 1;
                            }
                        }
                    }
                }
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
    inline InstanceBasedProblem::Constructors<submodular::MaxCoverage, int, int>
    InstanceBasedProblem::load_instances<submodular::MaxCoverage>(
        const std::optional<fs::path> &definitions_file)
    {
        using namespace submodular;
        return GraphProblemType<MaxCoverage>::get_constructors(
            definitions_file.value_or(common::file::utils::find_static_file("example_list_maxcoverage")));
    }


} // namespace ioh::problem
