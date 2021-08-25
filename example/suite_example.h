#pragma once

#include "ioh.hpp"

/// Declare a bbob suite of problem {1,2}, instance {1, 2} and dimension {5,10}.
/// We can use either factory construction, or direct object construction.
inline std::shared_ptr<ioh::suite::Suite<ioh::problem::Real>> create_suite(const bool using_factory = true)
{
    const std::vector<int> problems{1, 2};
    const std::vector<int> instances{1, 2};
    const std::vector<int> dimensions{1, 2};

    if (using_factory)
        return ioh::suite::SuiteRegistry<ioh::problem::Real>::instance()
            .create("BBOB", problems, instances, dimensions);
    return std::make_shared<ioh::suite::BBOB>(problems, instances, dimensions);
}


/// An example of using a suite class of problems.
inline void suite_example()
{
    const auto bbob = create_suite();

    std::cout << "==========\nAn example of using bbob suite\n==========" << std::endl;

    /// We can directly loop over a suite: This loop automatically
    /// resets each problem after every iteration.
    for (const auto &problem : *bbob)
    {
        for (auto runs = 0; runs < 1; runs ++)
        {
            /// To output information of the current problem.
            std::cout << problem->meta_data() << std::endl;

            /// Random search on the problem with the given budget 100.
            auto best_y = std::numeric_limits<double>::infinity();
            for (auto budget = 100; budget > 0; budget--)
            {
                /// To evalute the fitness of 'x' for the problem by using '(*problem)(x)' function.
                best_y = std::min(
                    (*problem)(ioh::common::random::doubles(problem->meta_data().n_variables, -5, 5)),
                    best_y);
            }
                
            /// To reset evaluation information as default before the next independent run.
            std::cout << "result: " << best_y << std::endl;
        }
    }
}
