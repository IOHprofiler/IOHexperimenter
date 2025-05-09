#include <iostream>
#include <string>
#include "ioh.hpp"
#include <filesystem>

//! Compute the sum of a vector of double
inline std::vector<double> sum_vector(const std::vector<double> &x) { 
    return x;
}



//! Example of a variables transformation function
std::vector<double> transform_x(std::vector<double> x, const int iid){
    // x.at(0) = static_cast<double>(iid);
    return x;
}

//! Example of a objective transformation function
std::vector<double> transform_y(std::vector<double> y, const int iid){
    // for (auto &val : y) {
    //     val *= iid;
    // }
    return y;
}



int main() {
    
    ioh::problem::Solution<double, ioh::problem::MultiObjective> solution1;
    ioh::problem::Solution<double, ioh::problem::MultiObjective> solution2 {
        {1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}
    };
    ioh::problem::Solution<double, ioh::problem::MultiObjective> solution3 { 10,2, ioh::common::OptimizationType::MIN };
    
    

    printf("Solution 1: %s\n", solution1.repr().c_str());
    printf("Solution 2: %s\n", solution2.repr().c_str());

    printf("Solution 3: %s\n", solution3.repr().c_str());

    auto my_evaluation = std::function<std::vector<double>(const std::vector<double>&)>(sum_vector);
    auto variable_transform = std::function<std::vector<double>(std::vector<double>, int)>(transform_x);
    auto objective_transform = std::function<std::vector<double>(std::vector<double>, int)>(transform_y);

    auto problem = ioh::problem::MultiObjectiveWrappedProblem<double>(
        my_evaluation,
        "Element wise", 
        2, 
        2,
        1,
        1,
        ioh::common::OptimizationType::MIN,
        ioh::problem::Bounds<double>(),
        variable_transform,
        objective_transform,
        ioh::problem::Solution<double, ioh::problem::MultiObjective>({0,0}, {0,0})
    );
    const auto logger = std::make_shared<ioh::logger::MultiAnalyzer>();
    problem.attach_logger(*logger);
    std::cout << problem.meta_data() << std::endl;
    std::cout << problem.state().repr() << std::endl;

    auto result1 = problem({1.0, 2.0});
    for (const auto &val : result1) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    problem.invert();
    std::cout << problem.meta_data() << std::endl;
    std::cout << problem.state().repr() << std::endl;

    auto result2 = problem({1.0, 2.0});
    for (const auto &val : result2) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    problem.invert();
    std::cout << "START" << std::endl;
    std::cout << problem.state().repr() << std::endl;
    auto result = problem({1.0, 3.0});
    for (const auto &val : result) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    std::cout << problem.state().repr() << std::endl;

    result = problem({4.0, 2.0});
    for (const auto &val : result) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    std::cout << problem.state().repr() << std::endl;

    result = problem({5.0, 1.0});
    for (const auto &val : result) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    std::cout << problem.state().repr() << std::endl;
    
    result = problem({3.0, 0.0});
    for (const auto &val : result) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    std::cout << problem.state().repr() << std::endl;


    // result = problem({0.0, 0.0});
    // for (const auto &val : result) {
    //     std::cout << val << " ";
    // }
    // std::cout << std::endl;
    // std::cout << problem.state().repr() << std::endl;
    return 0;
}
