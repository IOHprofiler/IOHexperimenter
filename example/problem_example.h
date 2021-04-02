#include <iostream>
#include <random>
#include <algorithm>
#include <vector>

#include "ioh.hpp"

/// An example of using problem classes, and a default logger is used to store evaluation information.
void problem_example() {
    
    std::vector<int> instance{1,2,3,51,52};
    std::vector<int> dimension{50,100};
    
    /// To declare a csv logger class.
    /// output_directory : "./"
    /// folder_name : "problem_example"
    /// algorithm_name : "random search'
    /// algorithm_info : "a random search for testing OneMax"
    auto l = ioh::logger::Default("./", "problem_example", "random_search", "a random search for testing OneMax");
    
    std::cout << "==========\nAn example of testing onemax\n==========" << std::endl;
    
    for(std::vector<int>::iterator d = dimension.begin(); d != dimension.end(); ++d) {
        for(std::vector<int>::iterator i = instance.begin(); i != instance.end(); ++i) {
            
            const auto om = std::make_shared<ioh::problem::pbo::OneMax>(*i, *d);
            
            int runs = 1;
            while(runs-- > 0 ) {
                om->attach_logger(l);
                /// To output information of the current problem.
                std::cout << om->meta_data() << std::endl;;
                
                int budget = 100;
                auto n = om->meta_data().n_variables;
                
                /// Random search on the problem with the given budget 100.
                std::vector<int> x(n);
                std::vector<double> r(n);
                auto best_y = -std::numeric_limits<double>::infinity();
                while(budget > 0) {
                    budget--;
                    ioh::common::Random::uniform(n, budget * runs, r);
                    for (int i = 0; i != n; i++) {
                        x[i] = static_cast<int>(r[i] * 2);
                    }
                    
                    /// To evalute the fitness of 'x' for the problem by using '(*om)(x)' function.
                    best_y = std::max((*om)(x).at(0), best_y);
                }
                std::cout << "result: " << best_y << std::endl;
            }
        }
    }
    
}


