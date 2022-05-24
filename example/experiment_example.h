#pragma once

#include "ioh.hpp"

/// A solver for IOH real problems.
inline void solver(const std::shared_ptr<ioh::problem::Real> p)
{
    using namespace ioh::common;
    std::vector<double> x(p->meta_data().n_variables);
    auto count = 0;
    while (count++ < 100)
    {
        (*p)(random::doubles(p->meta_data().n_variables));
    }
}

/// An example of using the experiment class to test a 'solver' on the bbob suite.
inline void experiment_example()
{
    std::cout << "==========\nAn example of using experiment class\n"
              << "Output in the folder ioh_data\n==========" << std::endl;
    
    const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();
    const auto suite = suite_factory.create("BBOB", {1, 2}, {1, 2}, {5, 10});
    const auto logger = std::make_shared<ioh::logger::Analyzer>();

    /// Create a experiment with suite, logger for Algorithm: solver. 10 independent runs per
    /// problem/instance/dimension combination.
    ioh::Experimenter<ioh::problem::Real> f(suite, logger, solver, 10);
    f.run();
}