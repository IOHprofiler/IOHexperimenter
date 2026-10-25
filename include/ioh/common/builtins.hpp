#pragma once

#include <vector>

// The factories that have built-in entries, and how a factory gets them.
//
// A factory adds its built-in entries when it is created (the first call of Factory::instance()) by calling
// add_builtins on itself. For the factories listed below, add_builtins is defined in the compiled ioh library
// (src/registry), so that only the library instantiates the problem and suite classes; a program without the library
// fails to link instead of getting empty factories. Every other factory starts empty. Every translation unit that
// creates a factory must see the same list, so it lives here, before the Factory definition, and not next to the
// problem classes.

namespace ioh::problem
{
    template <typename T>
    class SingleObjectiveProblem;
    class BBOB;
    struct SBOX;
    class PBO;
    class CEC2013;
    class CEC2022;
    class DynamicBinVal;

    namespace submodular
    {
        struct GraphProblem;
    }

    namespace star_discrepancy::real
    {
        class StarDiscrepancy;
    }

    namespace star_discrepancy::integer
    {
        class StarDiscrepancy;
    }
} // namespace ioh::problem

namespace ioh::suite
{
    template <typename ProblemType>
    class Suite;
}

namespace ioh::common
{
    template <class AbstractType, typename... Args>
    struct Factory;

    //! Factories without built-in entries start empty
    template <class AbstractType, typename... Args>
    void add_builtins(Factory<AbstractType, Args...> &)
    {
    }

    //! @name The factories with built-in entries
    ///@{

    //! All real single-objective problems: BBOB, CEC2013, CEC2022, real star discrepancy
    void add_builtins(Factory<problem::SingleObjectiveProblem<double>, int, int> &);

    //! All integer single-objective problems: PBO, graph, integer star discrepancy, DynamicBinVal
    void add_builtins(Factory<problem::SingleObjectiveProblem<int>, int, int> &);

    void add_builtins(Factory<problem::BBOB, int, int> &);
    void add_builtins(Factory<problem::SBOX, int, int> &);
    void add_builtins(Factory<problem::PBO, int, int> &);
    void add_builtins(Factory<problem::CEC2013, int, int> &);
    void add_builtins(Factory<problem::CEC2022, int, int> &);
    void add_builtins(Factory<problem::DynamicBinVal, int, int> &);
    void add_builtins(Factory<problem::submodular::GraphProblem, int, int> &);
    void add_builtins(Factory<problem::star_discrepancy::real::StarDiscrepancy, int, int> &);
    void add_builtins(Factory<problem::star_discrepancy::integer::StarDiscrepancy, int, int> &);

    //! The real suites: Real, BBOB, SBOX, RealStarDiscrepancy
    void add_builtins(Factory<suite::Suite<problem::SingleObjectiveProblem<double>>, std::vector<int>, std::vector<int>,
                              std::vector<int>> &);

    //! The integer suites: Integer, PBO, Submodular, IntegerStarDiscrepancy
    void add_builtins(Factory<suite::Suite<problem::SingleObjectiveProblem<int>>, std::vector<int>, std::vector<int>,
                              std::vector<int>> &);
    ///@}
} // namespace ioh::common
