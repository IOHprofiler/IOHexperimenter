#include <iostream>
#include "ioh.hpp"

bool float_eq(const double a, const double b)
{
    return fabs(a - b) < 1e-8;
}


void show_registered_objects()
{
    {
        const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::Real>::instance();
        const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();

        std::cout << "Registered Real Problems:\n";

        for (auto &[id, name] : problem_factory.name_to_id())
            std::cout << id << ", " << name << std::endl;

        std::cout << "\nRegistered Real Suites:\n";
        for (auto &[id, name] : suite_factory.name_to_id())
            std::cout << id << ", " << name << std::endl;
    }
    {
        const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::Integer>::instance();
        const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Integer>::instance();

        std::cout << "\nRegistered Integer Problems:\n";

        for (auto &[id, name] : problem_factory.name_to_id())
            std::cout << id << ", " << name << std::endl;

        std::cout << "\nRegistered Integer Suites:\n";
        for (auto &[id, name] : suite_factory.name_to_id())
            std::cout << id << ", " << name << std::endl;
    }
}

void test_logger()
{
    const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();
    const auto suite = suite_factory.create(
        "BBOB", {1, 2}, {1, 2}, {5});

    std::vector<double> x0{0.1, 1., 2., 4., 5.4};
    auto logger = ioh::logger::Default(std::string("logger1"));
    suite->attach_logger(logger);

    for (const auto &p : *suite)
        for (auto i = 0; i < 3; i++)
        {
            for (auto j = 0; j < 10; j++)
            {
                x0.at(i) -= static_cast<double>(j);
                (*p)(x0);
            }
            p->reset();
        }
}


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

void test_experimenter()
{
    const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();
    const auto suite = suite_factory.create("BBOB", {1, 2}, {1, 2}, {5, 10});
    const auto logger = std::make_shared<ioh::logger::Default>(std::string("logger-experimenter"));

    ioh::experiment::Experimenter<ioh::problem::Real> f(suite, logger, solver, 10);
    f.run();
}

class AnotherRealProblem final: public ioh::problem::RealProblem<AnotherRealProblem>
{
protected:
    std::vector<double> evaluate(const std::vector<double> &x) override
    {
        return {x.at(0)};
    }

public:
    AnotherRealProblem(const int instance, const int n_variables):
        RealProblem(ioh::problem::MetaData(1, instance,
                                           "AnotherRealProblem", n_variables, 1,
                                           ioh::common::OptimizationType::Minimization))
    {
    }
};


std::vector<double> f(const std::vector<double>& x)
{
    return { 0.0 };
}


int main()
{
    // show_registered_objects();
    // test_logger();
    // test_ecdf();
    // test_experimenter();
    using RealFactory = ioh::problem::ProblemFactoryType <ioh::problem::Real>;
    // using BBOBFactory = ioh::problem::ProblemFactoryType < ioh::problem::BBOB>;
    //
    RealFactory& factory = RealFactory::instance();
    //
    // reinterpret_cast<RealFactory&>(f);
    //
    // ioh::suite::Real r({ 1 }, { 2 }, { 5 });
    // ioh::suite::BBOB b({1}, {2}, {5});

    auto p = ioh::problem::wrap_function<double>(f, "f", 10);


    auto p2 = factory.create("f", 5, 1);



    std::cout << " done";
}
