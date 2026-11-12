// Built-in entries of the adversarial attack factory
#include "families.hpp"

namespace ioh::common
{
    void add_builtins(Factory<problem::adversarial::AdversarialAttack, int, int> &factory)
    {
        registry::add_adversarial(factory);
    }
} // namespace ioh::common
