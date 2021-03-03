#include <iostream>
#include "ioh/problem.hpp"
#include "ioh/suite.hpp"
#include "ioh/logger.hpp"

bool float_eq(const double a, const double b)
{
    return fabs(a - b) < 1e-8;
}

void test_problems()
{
    const std::vector<double> expected{
        161.17445568,
        12653420.412225708,
        -97.06158297486468,
        8845.53524810093,
        121.9133638725643,
        423021.00682286796,
        278.3290201933729,
        4315.032550201522,
        95168.25362963174,
        969025.0451803299,
        20911310.769634742,
        187251637.66430587,
        2198.7155122256763,
        261.60997479957,
        1103.1553337856833,
        273.87870697831676,
        35.41073948743459,
        107.64134358768,
        -40.26919893274103,
        49739.05388887795,
        116.29585727504872,
        -914.902473409051,
        18.635078550305444,
        1782.2733296400438,
    };


    const auto dimension = 5;

    const auto& the_factory = ioh::problem::ProblemRegistry<ioh::problem::RealProblem>::instance();

    const std::vector<std::shared_ptr<ioh::problem::RealProblem>> items = {
         the_factory.create("Sphere", 1, dimension),
         the_factory.create("Ellipsoid", 1, dimension),
         the_factory.create("Rastrigin", 1, dimension),
         the_factory.create("BuecheRastrigin", 1, dimension),
         the_factory.create("LinearSlope", 1, dimension),
         the_factory.create("AttractiveSector", 1, dimension),
         the_factory.create("StepEllipsoid", 1, dimension),
         the_factory.create("Rosenbrock", 1, dimension),
         the_factory.create("RosenbrockRotated", 1, dimension),
         the_factory.create("EllipsoidRotated", 1, dimension),
         the_factory.create("Discus", 1, dimension),
         the_factory.create("BentCigar", 1, dimension),
         the_factory.create("SharpRidge", 1, dimension),
         the_factory.create("DifferentPowers", 1, dimension),
         the_factory.create("RastriginRotated", 1, dimension),
         the_factory.create("Weierstrass", 1, dimension),
         the_factory.create("Schaffers10", 1, dimension),
         the_factory.create("Schaffers1000", 1, dimension),
         the_factory.create("GriewankRosenBrock", 1, dimension),
         the_factory.create("Schwefel", 1, dimension),
         the_factory.create("Gallagher101", 1, dimension),
         the_factory.create("Gallagher21", 1, dimension),
         the_factory.create("Katsuura", 1, dimension),
         the_factory.create("LunacekBiRastrigin", 1, dimension),
    };


    // TODO: check weierstrass, xopt in: rosenbrock
    const std::vector<double> x0{ 0.1, 1., 2.,4., 5.4 };
   
    for (auto i = 0; i < items.size(); i++)
    {
        const auto item = items.at(i);
        const auto result = expected.at(i);
        const auto y0 = (*item)(x0).at(0);

        const auto yopt = (*item)(item->objective().x).at(0);
        if (!float_eq(result, y0) || !float_eq(item->objective().y.at(0), yopt))
        {
            std::cout << i + 1 << ": " << item->meta_data().name << std::endl;
            std::cout << item->objective()  << " == " << yopt << " = " << std::boolalpha
                << float_eq(item->objective().y.at(0), yopt) << std::endl;

            std::cout << result << " == " << y0 << " = " << std::boolalpha << float_eq(result, y0) << std::endl;
        }
    }


    ioh::suite::BBOB suite({ 1, 2 }, { 1, 2 }, { 5 });

    std::cout << suite.name() << std::endl;
    for (const auto& p : suite)
        std::cout << *p << std::endl;
    
    std::vector<int> pbo_ids(10);
    std::iota(pbo_ids.begin(), pbo_ids.end(), 1);
    ioh::suite::PBO suite2(pbo_ids, { 1, 2 }, { 16 });
    
    std::cout << suite2.name() << std::endl;
    for (const auto& p : suite2)
        std::cout << *p << std::endl;
    
    auto& suite_factory = ioh::suite::SuiteRegistry<ioh::suite::RealSuite>::instance();
    auto f = suite_factory.create("BBOB", { 1 }, { 2 }, { 5 });
    
    for (const auto& p : *f)
        std::cout << (*p)(x0).at(0) << std::endl;
}



int main() {

    using namespace ioh::common;
    test_problems();

    // const auto& the_factory = ioh::problem::ProblemRegistry<ioh::problem::PBO>::instance();
    //
    // for (auto& name : the_factory.names())
    //     std::cout << name << std::endl;
    //
    // std::cout << the_factory.names().size();
    //
    // std::vector<double> x0{ 0.1, 1., 2.,4., 5.4 };
    // // const auto problem = the_factory.create("Sphere", 1, 5);
    //
    // auto logger1 = ioh::logger::Default(std::string("logger1"));
    // // auto logger2 = ioh::logger::Default(std::string("logger2"));
    // // auto logger = ioh::logger::LoggerCombine({ &logger1, &logger2 });
    //  
    // // problem->attach_logger(logger1);
    // //
    //
    // //
    // // for (auto i =0; i < 10; i++)
    // // {
    // //     for (auto j = 0; j < 1000; j++)
    // //         (*problem)(x0);
    // //
    // //     problem->reset();
    // // }
    //
    // auto& suite_factory = ioh::suite::SuiteRegistry<ioh::suite::RealSuite>::instance();
    // const auto suite = suite_factory.create("BBOB", { 1 }, { 1, 2 }, { 5 });
    //
    // suite->attach_logger(logger1);
    //
    // for (const auto& p : *suite)
    // {
    //     std::cout << *p << std::endl;
    //
    //     for (auto i = 0; i < 2; i++)
    //     {
    //          for (auto j = 0; j < 10; j++)
    //          {
    //              x0.at(0) += static_cast<double>(j);
    //              (*p)(x0);
    //          }
    //          p->reset();
    //     }
    // }

    std::cout << "done";
}

