// Built-in entries of the BBOB and SBOX factories, and the instantiations of the BBOB problem classes that
// their headers declare extern
#include "families.hpp"

namespace ioh::problem::bbob
{
    template class Sphere<BBOB>;
    template class Sphere<SBOX>;
    template class Ellipsoid<BBOB>;
    template class Ellipsoid<SBOX>;
    template class Rastrigin<BBOB>;
    template class Rastrigin<SBOX>;
    template class BuecheRastrigin<BBOB>;
    template class BuecheRastrigin<SBOX>;
    template class LinearSlope<BBOB>;
    template class LinearSlope<SBOX>;
    template class AttractiveSector<BBOB>;
    template class AttractiveSector<SBOX>;
    template class StepEllipsoid<BBOB>;
    template class StepEllipsoid<SBOX>;
    template class Rosenbrock<BBOB>;
    template class Rosenbrock<SBOX>;
    template class RosenbrockRotated<BBOB>;
    template class RosenbrockRotated<SBOX>;
    template class EllipsoidRotated<BBOB>;
    template class EllipsoidRotated<SBOX>;
    template class Discus<BBOB>;
    template class Discus<SBOX>;
    template class BentCigar<BBOB>;
    template class BentCigar<SBOX>;
    template class SharpRidge<BBOB>;
    template class SharpRidge<SBOX>;
    template class DifferentPowers<BBOB>;
    template class DifferentPowers<SBOX>;
    template class RastriginRotated<BBOB>;
    template class RastriginRotated<SBOX>;
    template class Weierstrass<BBOB>;
    template class Weierstrass<SBOX>;
    template class Schaffers10<BBOB>;
    template class Schaffers10<SBOX>;
    template class Schaffers1000<BBOB>;
    template class Schaffers1000<SBOX>;
    template class GriewankRosenbrock<BBOB>;
    template class GriewankRosenbrock<SBOX>;
    template class Schwefel<BBOB>;
    template class Schwefel<SBOX>;
    template class Gallagher101<BBOB>;
    template class Gallagher101<SBOX>;
    template class Gallagher21<BBOB>;
    template class Gallagher21<SBOX>;
    template class Katsuura<BBOB>;
    template class Katsuura<SBOX>;
    template class LunacekBiRastrigin<BBOB>;
    template class LunacekBiRastrigin<SBOX>;
} // namespace ioh::problem::bbob

namespace ioh::common
{
    void add_builtins(Factory<problem::BBOB, int, int> &factory) { registry::add_bbob<problem::BBOB>(factory); }

    void add_builtins(Factory<problem::SBOX, int, int> &factory) { registry::add_bbob<problem::SBOX>(factory); }
} // namespace ioh::common
