// Built-in entries of the graph problem factory
#include "families.hpp"

namespace ioh::common
{
    void add_builtins(Factory<problem::submodular::GraphProblem, int, int> &factory) { registry::add_graph(factory); }
} // namespace ioh::common
