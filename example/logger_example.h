#include <iostream>
#include <random>
#include <algorithm>
#include <vector>

#include "ioh.hpp"

/// An example of using the default logger class to store evaluation information during the optimization process.
void logger_example() {
    
    /// To declare a csv logger class.
    /// output_directory : "./"
    /// folder_name : "logger_example"
    /// algorithm_name : "random search'
    /// algorithm_info : "a random search for testing the bbob suite"
    auto l = ioh::logger::Default("./", "logger_example", "random_search", "a random search for testing the bbob suite");
    
    /// To declare a bbob suite of problem {1,2}, intance {1, 2} and dimension {5,10}.
    const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();
    const auto bbob = suite_factory.create("BBOB", {1, 2}, {1, 2}, {5, 10});
    
    std::cout << "==========\nAn example of testing logger\n==========" << std::endl;
    
    /// To access problems of the suite.
    for (const auto &problem : *bbob) {
        problem->attach_logger(l);
        int budget = 100;
        auto n = problem->meta_data().n_variables;
        
        /// To output information of the current problem.
        std::cout<< problem->meta_data() <<std::endl;
        
        /// Random search on the problem with the given budget 100.
        std::vector<double> x(n);
        auto best_y = std::numeric_limits<double>::infinity();
        while(budget > 0) {
            budget--;
            for (int i = 0; i != n; i++) {
                ioh::common::Random::uniform(n, budget, x);
                x[i] = x[i] * 10 - 5;
            }
            
            /// To evalute the fitness of 'x' for the problem by using '(*problem)(x)' function.
            best_y = std::min((*problem)(x).at(0), best_y);
        }
        std::cout << "result: " << best_y << std::endl;
    }
}


