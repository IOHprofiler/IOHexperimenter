#include <iostream>

#include "ioh/problem.hpp"


int main() {


    const auto& the_factory = ioh::problem::Factory<ioh::problem::RealProblem>::instance();

    const int dimension = 5;
    const auto items = {
         // the_factory.create("Sphere", 1, dimension),
         // the_factory.create("Ellipsoid", 1, dimension),
         // the_factory.create("Rastrigin", 1, dimension),
         // the_factory.create("BuecheRastrigin", 1, dimension),
         // the_factory.create("LinearSlope", 1, dimension),
         // the_factory.create("AttractiveSector", 1, dimension),
         the_factory.create("StepEllipsoid", 1, dimension),
        
   }; 

    const std::vector<double> x0{ 0.1, 1., 2.,4., 5.4 };

    for (const auto& i : items)
    {
        std::cout << *i << std::endl;
        std::cout << i->meta_data().objective << std::endl;
        auto y0 = (*i)(x0).at(0);
        std::cout << (*i)(i->meta_data().objective.x).at(0) << ", x0: ";
        std::cout << y0 << std::endl;
    }

    std::cout << "done";
}
