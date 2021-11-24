#include "experiment_example.h"
#include "logger_example.h"
#include "problem_example.h"
#include "suite_example.h"

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

double fn(const std::vector<double> &x) { 
    return std::accumulate(x.begin(), x.end(), 0.0); 
}

ioh::problem::Solution<double> calculate_objective(const int iid, const int dim)
{
    return {std::vector<double>(dim, iid), static_cast<double>(iid * dim)};
}

std::vector<double> tx(std::vector<double> x, const int iid){
    x.at(0) = static_cast<double>(iid);
    return x;
}

double ty(const double y, const int iid){
    return y * iid;
}


int main()
{
    // show_registered_objects();
    // suite_example();
    // logger_example();
    // problem_example();
    // experiment_example();
    using namespace ioh::common;
    using namespace ioh::problem;

    auto &factory = ProblemFactoryType<Problem<double>>::instance();
    wrap_function<double>(fn, "fn", OptimizationType::Minimization, -5, 5, 
                        tx, ty, calculate_objective);

    std::vector<double> x0 = {1, 0, 2.5};

    for (auto inst: {1, 2, 3}){
        auto problem = factory.create("fn", inst, 3);
        std::cout << *problem << std::endl;
        std::cout << (*problem)(x0) << std::endl;
    }
}
