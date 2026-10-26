#pragma once

// The built-in problem families, written once and composed into the factories by the other files in this
// directory. Only these files instantiate the problem classes for the factories.

#include "ioh.hpp"

namespace ioh::registry
{
    //! Adds registry entries to a problem factory
    template <class Parent, class T>
    void add_entries(common::Factory<Parent, int, int> &factory, const std::vector<problem::RegistryEntry<T>> &entries)
    {
        for (const auto &entry : entries)
            factory.include(entry.name, entry.id,
                            [create = entry.create](int &&instance, int &&n_variables) -> std::shared_ptr<Parent> {
                                return create(instance, n_variables);
                            });
    }

    //! Adds the problem class T to a problem factory: its registry entries, or its registry_id and registry_name. The
    //! same as add_entries(factory, problem::registry_entries_of<T>()), but a class-backed creator is wrapped in one
    //! std::function instead of two (which made this library 30-45 % slower to compile)
    template <class Parent, class T>
    void add_class(common::Factory<Parent, int, int> &factory)
    {
        if constexpr (problem::detail::has_registry_entries<T>::value)
            add_entries(factory, T::registry_entries());
        else
            factory.include(T::registry_name, T::registry_id,
                            [](int &&instance, int &&n_variables) -> std::shared_ptr<Parent> {
                                return std::make_shared<T>(instance, n_variables);
                            });
    }

    //! A list of types: the classes of a problem family, also used by the Python bindings to define their classes
    template <class... Ts>
    struct type_list
    {
    };

    //! Adds the problem classes of a type list to a problem factory
    template <class Parent, class... Ts>
    void add(common::Factory<Parent, int, int> &factory, type_list<Ts...>)
    {
        (add_class<Parent, Ts>(factory), ...);
    }

    //! Adds the suite classes Ts, with their registry_id and registry_name, to a suite factory (as users register
    //! theirs, see suite::register_suite)
    template <class ProblemType, class... Ts>
    void add_suites(suite::SuiteFactoryType<ProblemType> &factory)
    {
        (suite::register_suite<Ts>(factory), ...);
    }

    //! The 24 BBOB functions (ids 1-24) in the variant V, problem::BBOB or problem::SBOX
    template <class V>
    using bbob_classes = type_list<
        problem::bbob::Sphere<V>, problem::bbob::Ellipsoid<V>, problem::bbob::Rastrigin<V>,
        problem::bbob::BuecheRastrigin<V>, problem::bbob::LinearSlope<V>, problem::bbob::AttractiveSector<V>,
        problem::bbob::StepEllipsoid<V>, problem::bbob::Rosenbrock<V>, problem::bbob::RosenbrockRotated<V>,
        problem::bbob::EllipsoidRotated<V>, problem::bbob::Discus<V>, problem::bbob::BentCigar<V>,
        problem::bbob::SharpRidge<V>, problem::bbob::DifferentPowers<V>, problem::bbob::RastriginRotated<V>,
        problem::bbob::Weierstrass<V>, problem::bbob::Schaffers10<V>, problem::bbob::Schaffers1000<V>,
        problem::bbob::GriewankRosenbrock<V>, problem::bbob::Schwefel<V>, problem::bbob::Gallagher101<V>,
        problem::bbob::Gallagher21<V>, problem::bbob::Katsuura<V>, problem::bbob::LunacekBiRastrigin<V>>;

    //! The 25 PBO problems (ids 1-25)
    using pbo_classes = type_list<
        problem::pbo::OneMax, problem::pbo::LeadingOnes, problem::pbo::Linear, problem::pbo::OneMaxDummy1,
        problem::pbo::OneMaxDummy2, problem::pbo::OneMaxNeutrality, problem::pbo::OneMaxEpistasis,
        problem::pbo::OneMaxRuggedness1, problem::pbo::OneMaxRuggedness2, problem::pbo::OneMaxRuggedness3,
        problem::pbo::LeadingOnesDummy1, problem::pbo::LeadingOnesDummy2, problem::pbo::LeadingOnesNeutrality,
        problem::pbo::LeadingOnesEpistasis, problem::pbo::LeadingOnesRuggedness1, problem::pbo::LeadingOnesRuggedness2,
        problem::pbo::LeadingOnesRuggedness3, problem::pbo::LABS, problem::pbo::IsingRing, problem::pbo::IsingTorus,
        problem::pbo::IsingTriangular, problem::pbo::MIS, problem::pbo::NQueens, problem::pbo::ConcatenatedTrap,
        problem::pbo::NKLandscapes>;

    //! The 12 CEC2022 problems (ids 1001-1012)
    using cec2022_classes = type_list<
        problem::cec2022::Zakharov, problem::cec2022::Rosenbrock, problem::cec2022::SchafferF7,
        problem::cec2022::Rastrigin, problem::cec2022::Levy, problem::cec2022::HybridFunction1,
        problem::cec2022::HybridFunction2, problem::cec2022::HybridFunction3, problem::cec2022::CompositionFunction1,
        problem::cec2022::CompositionFunction2, problem::cec2022::CompositionFunction3,
        problem::cec2022::CompositionFunction4>;

    //! The 20 CEC2013 problems (ids 1101-1120) of 9 classes
    using cec2013_classes = type_list<
        problem::cec2013::FivePeaks, problem::cec2013::EqualMaxima, problem::cec2013::UnevenEqualMaxima,
        problem::cec2013::Himmelblau, problem::cec2013::SixHumpCamelback, problem::cec2013::Shubert,
        problem::cec2013::Vincent, problem::cec2013::ModifiedRastrigin, problem::cec2013::CompositionFunction>;

    //! The 4 DynamicBinVal problems (ids 10001-10004)
    using dynamic_bin_val_classes =
        type_list<problem::dynamic_bin_val::Uniform, problem::dynamic_bin_val::PowersOfTwo,
                  problem::dynamic_bin_val::Pareto, problem::dynamic_bin_val::Ranking>;

    //! The 78 graph problems (ids 2000-2308) of 4 classes, one per graph in the static data
    using graph_classes = type_list<problem::submodular::MaxCut, problem::submodular::MaxCoverage,
                                    problem::submodular::MaxInfluence, problem::submodular::PackWhileTravel>;

    //! Adds the 24 BBOB functions in the variant V, problem::BBOB or problem::SBOX
    template <class V, class Parent>
    void add_bbob(common::Factory<Parent, int, int> &factory)
    {
        add(factory, bbob_classes<V>{});
    }

    //! Adds the 25 PBO problems
    template <class Parent>
    void add_pbo(common::Factory<Parent, int, int> &factory)
    {
        add(factory, pbo_classes{});
    }

    //! Adds the 12 CEC2022 problems
    template <class Parent>
    void add_cec2022(common::Factory<Parent, int, int> &factory)
    {
        add(factory, cec2022_classes{});
    }

    //! Adds the 20 CEC2013 problems
    template <class Parent>
    void add_cec2013(common::Factory<Parent, int, int> &factory)
    {
        add(factory, cec2013_classes{});
    }

    //! Adds the 4 DynamicBinVal problems
    template <class Parent>
    void add_dynamic_bin_val(common::Factory<Parent, int, int> &factory)
    {
        add(factory, dynamic_bin_val_classes{});
    }

    //! Adds the 78 graph problems
    template <class Parent>
    void add_graph(common::Factory<Parent, int, int> &factory)
    {
        add(factory, graph_classes{});
    }

    //! The 30 real star discrepancy problems (ids 30-59)
    template <class Parent>
    void add_star_discrepancy_real(common::Factory<Parent, int, int> &factory)
    {
        add(factory, type_list<problem::star_discrepancy::real::StarDiscrepancy>{});
    }

    //! The 30 integer star discrepancy problems (ids 30-59)
    template <class Parent>
    void add_star_discrepancy_integer(common::Factory<Parent, int, int> &factory)
    {
        add(factory, type_list<problem::star_discrepancy::integer::StarDiscrepancy>{});
    }
} // namespace ioh::registry
