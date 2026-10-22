#pragma once

// The built-in problem families, written once and composed into the factories by the other files in this
// directory. Only these files instantiate the problem classes for the factories.

#include "ioh.hpp"

namespace ioh::registry
{
    /**
     * @brief Adds the problem classes Ts to a problem factory
     *
     * As before the registry was compiled: a class-backed problem gets the id and name of T(1, 1).meta_data(),
     * an instance-based problem (CEC2013, graph, star discrepancy) the entries of its load_instances.
     */
    template <class Parent, class... Ts>
    void add(common::Factory<Parent, int, int> &factory)
    {
        (common::RegisterWithFactory<Parent, int, int>::template include<Ts>(factory), ...);
    }

    //! The 24 BBOB functions (ids 1-24) in the variant V, problem::BBOB or problem::SBOX
    template <class V, class Parent>
    void add_bbob(common::Factory<Parent, int, int> &factory)
    {
        using namespace problem::bbob;
        add<Parent, Sphere<V>, Ellipsoid<V>, Rastrigin<V>, BuecheRastrigin<V>, LinearSlope<V>, AttractiveSector<V>,
            StepEllipsoid<V>, Rosenbrock<V>, RosenbrockRotated<V>, EllipsoidRotated<V>, Discus<V>, BentCigar<V>,
            SharpRidge<V>, DifferentPowers<V>, RastriginRotated<V>, Weierstrass<V>, Schaffers10<V>, Schaffers1000<V>,
            GriewankRosenbrock<V>, Schwefel<V>, Gallagher101<V>, Gallagher21<V>, Katsuura<V>, LunacekBiRastrigin<V>>(
            factory);
    }

    //! The 25 PBO problems (ids 1-25)
    template <class Parent>
    void add_pbo(common::Factory<Parent, int, int> &factory)
    {
        using namespace problem::pbo;
        add<Parent, OneMax, LeadingOnes, Linear, OneMaxDummy1, OneMaxDummy2, OneMaxNeutrality, OneMaxEpistasis,
            OneMaxRuggedness1, OneMaxRuggedness2, OneMaxRuggedness3, LeadingOnesDummy1, LeadingOnesDummy2,
            LeadingOnesNeutrality, LeadingOnesEpistasis, LeadingOnesRuggedness1, LeadingOnesRuggedness2,
            LeadingOnesRuggedness3, LABS, IsingRing, IsingTorus, IsingTriangular, MIS, NQueens, ConcatenatedTrap,
            NKLandscapes>(factory);
    }

    //! The 12 CEC2022 problems (ids 1001-1012)
    template <class Parent>
    void add_cec2022(common::Factory<Parent, int, int> &factory)
    {
        using namespace problem::cec2022;
        add<Parent, Zakharov, Rosenbrock, SchafferF7, Rastrigin, Levy, HybridFunction1, HybridFunction2,
            HybridFunction3, CompositionFunction1, CompositionFunction2, CompositionFunction3, CompositionFunction4>(
            factory);
    }

    //! The 20 CEC2013 problems (ids 1101-1120) of 9 classes
    template <class Parent>
    void add_cec2013(common::Factory<Parent, int, int> &factory)
    {
        using namespace problem::cec2013;
        add<Parent, FivePeaks, EqualMaxima, UnevenEqualMaxima, Himmelblau, SixHumpCamelback, Shubert, Vincent,
            ModifiedRastrigin, CompositionFunction>(factory);
    }

    //! The 4 DynamicBinVal problems (ids 10001-10004)
    template <class Parent>
    void add_dynamic_bin_val(common::Factory<Parent, int, int> &factory)
    {
        using namespace problem::dynamic_bin_val;
        add<Parent, Uniform, PowersOfTwo, Pareto, Ranking>(factory);
    }

    //! The 78 graph problems (ids 2000-2308) of 4 classes, one per graph in the static data
    template <class Parent>
    void add_graph(common::Factory<Parent, int, int> &factory)
    {
        using namespace problem::submodular;
        add<Parent, MaxCut, MaxCoverage, MaxInfluence, PackWhileTravel>(factory);
    }

    //! The 30 real star discrepancy problems (ids 30-59)
    template <class Parent>
    void add_star_discrepancy_real(common::Factory<Parent, int, int> &factory)
    {
        add<Parent, problem::star_discrepancy::real::StarDiscrepancy>(factory);
    }

    //! The 30 integer star discrepancy problems (ids 30-59)
    template <class Parent>
    void add_star_discrepancy_integer(common::Factory<Parent, int, int> &factory)
    {
        add<Parent, problem::star_discrepancy::integer::StarDiscrepancy>(factory);
    }
} // namespace ioh::registry
