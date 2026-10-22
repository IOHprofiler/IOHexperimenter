// Built-in entries of the DynamicBinVal factory
#include "families.hpp"

namespace ioh::common
{
    void add_builtins(Factory<problem::DynamicBinVal, int, int> &factory) { registry::add_dynamic_bin_val(factory); }
} // namespace ioh::common
