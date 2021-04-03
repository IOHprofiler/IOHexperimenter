#include <iostream>
#include <random>
#include <algorithm>
#include <vector>

#include "ioh.hpp"

/// A solver for IOH real problems.
void solver(const std::shared_ptr<ioh::problem::Real> p)
{
    using namespace ioh::common;
    std::vector<double> x(p->meta_data().n_variables);
    auto count = 0;
    while (count++ < 100)
    {
        Random::uniform(p->meta_data().n_variables, Random::integer(), x);
        (*p)(x);
    }
}

/// An example of using the experiment class to test a 'solver' on the bbob suite.
void experiment_example()
{
    std::cout << "==========\nAn example of using experiment class" << std::endl;
    std::cout << "Output in the folder logger-experimenter\n==========" << std::endl;
    const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();
    const auto suite = suite_factory.create("BBOB", {1, 2}, {1, 2}, {5, 10});
    const auto logger = std::make_shared<ioh::logger::Default>(std::string("logger-experimenter"));

    ioh::experiment::Experimenter<ioh::problem::Real> f(suite, logger, solver, 10);
    f.run();
}
