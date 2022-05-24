#include "ioh.hpp"


void show_registered_objects()
{
    {
        const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::Real>::instance();
        const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();

        std::cout << "Registered Real Problems:\n";

        for (auto &[id, name] : problem_factory.map())
            std::cout << id << ", " << name << std::endl;

        std::cout << "\nRegistered Real Suites:\n";
        for (auto &[id, name] : suite_factory.map())
            std::cout << id << ", " << name << std::endl;
    }
    {
        const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::Integer>::instance();
        const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Integer>::instance();

        std::cout << "\nRegistered Integer Problems:\n";

        for (auto &[id, name] : problem_factory.map())
            std::cout << id << ", " << name << std::endl;

        std::cout << "\nRegistered Integer Suites:\n";
        for (auto &[id, name] : suite_factory.map())
            std::cout << id << ", " << name << std::endl;
    }
}


// Example
int main()
{
    using namespace ioh::problem;
    show_registered_objects();
    // we first create a problem factory, only for GraphProblem Types
    const auto &problem_factory = ProblemRegistry<submodular::v2::GraphProblem>::instance();

    // We loop over all problems, for now I only did the 3 types.
    for (auto &[id, name] : problem_factory.map())
    {
        // Creating a problem instance
        // This is slower the first time the problem is created, as the graph has to be loaded from file
        // subsequent calls are faster. 
        auto problem = problem_factory.create(id, 1, 1);
        std::cout << problem->meta_data() << std::endl;

        // We can then call the problem with an array of integers a would be expected
        auto x0 = ioh::common::random::integers(problem->meta_data().n_variables, 0, 1);
        std::cout << "y0: " << (*problem)(x0) << std::endl;
    }

    // Alternatively, we can create a suite
    ioh::suite::Submodular suite;
    for (const auto& problem: suite){
        std::cout << (*problem).meta_data() << std::endl;
    }
}
