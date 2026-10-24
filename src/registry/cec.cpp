// Built-in entries of the CEC2013 and CEC2022 factories
#include "families.hpp"

namespace ioh::common
{
    void add_builtins(Factory<problem::CEC2013, int, int> &factory) { registry::add_cec2013(factory); }

    void add_builtins(Factory<problem::CEC2022, int, int> &factory) { registry::add_cec2022(factory); }
} // namespace ioh::common
