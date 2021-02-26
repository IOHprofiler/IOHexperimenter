#include <iostream>
#include "ioh/problem.hpp"

int main() {
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

    const auto& the_factory = ioh::problem::Factory<ioh::problem::RealProblem>::instance();

    const int dimension = 5;


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
         the_factory.create("Schwefel", 1, dimension),
         the_factory.create("Schwefel", 1, dimension),
         the_factory.create("Katsuura", 1, dimension),
         the_factory.create("LunacekBiRastrigin", 1, dimension),
   }; 

    const std::vector<double> x0{ 0.1, 1., 2.,4., 5.4 };

    for (auto i = 0; i < items.size(); i++)
    {
        const auto item = items.at(i);
        const auto result = expected.at(i);
        const auto y0 = (*item)(x0).at(0);

        const auto yopt = (*item)(item->meta_data().objective.x).at(0);
        std::cout << i+1 << std::endl;
        std::cout << item->meta_data().objective << " == " << yopt << " = " << std::boolalpha
                  << (item->meta_data().objective.y.at(0) == yopt) << std::endl;
        
        std::cout << result << " == " << y0 << " = " << std::boolalpha << (result == y0) << std::endl;
    }


    std::cout << the_factory.keys().size() << std::endl;
    std::cout << "done";
}
