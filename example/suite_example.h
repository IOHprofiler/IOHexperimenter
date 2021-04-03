#include <iostream>
#include <random>
#include <algorithm>
#include <vector>

#include "ioh.hpp"

/// An example of using a suite class of problems.
void suite_example() {
    
    /// To declare a bbob suite of problem {1,2}, intance {1, 2} and dimension {5,10}.
    const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();
    const auto bbob = suite_factory.create("BBOB", {1, 2}, {1, 2}, {5, 10});
    
    std::cout << "==========\nAn example of using bbob suite\n==========" << std::endl;

    /// To access problems of the suite.
    for (const auto &problem : *bbob) {
        
        int runs = 1;
        while(runs-- > 0) {
            /// To output information of the current problem.
            std::cout<< problem->meta_data() <<std::endl;
            int budget = 100;
            auto n = problem->meta_data().n_variables;
            
            /// Random search on the problem with the given budget 100.
            std::vector<double> x(n);
            auto best_y = std::numeric_limits<double>::infinity();
            while(budget > 0) {
                budget--;
                for (int i = 0; i != n; i++) {
                    ioh::common::Random::uniform(n, budget * runs, x);
                    x[i] = x[i] * 10 - 5;
                }
                
                /// To evalute the fitness of 'x' for the problem by using '(*problem)(x)' function.
                best_y = std::min((*problem)(x).at(0), best_y);
            }
            /// To reset evaluation information as default before the next independent run.
            std::cout << "result: " << best_y << std::endl;
        }
    }
}


