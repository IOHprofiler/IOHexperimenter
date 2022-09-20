#include "ioh.hpp"
#include "problem_example.h"
#include "suite_example.h"
#include "logger_example.h"
#include "experiment_example.h"
// #include "fmt/format.h"

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

int main()
{
    // show_registered_objects();
    // problem_example();
    // suite_example();
    // experiment_example();
    // logger_example();

    // fmt::print("{}\n", std::numeric_limits<double>::infinity());
    
    // std::cout << fmt::format("{:.10f}", std::numeric_limits<double>::infinity()) << std::endl;
    // double d = std::numeric_limits<double>::infinity();

    // std::cout << fmt::format("{:.10f}", d) << std::endl;

    // d = -std::numeric_limits<double>::infinity();
    // std::cout << fmt::format("{:.10f}", d) << std::endl;

    auto p = ioh::problem::bbob::Sphere(1, 2);
    auto li = p.log_info();
    li.raw_y = p.meta_data().initial_objective_value;
    std::cout << ioh::watch::raw_y.call_to_string(li) << std::endl;
}

