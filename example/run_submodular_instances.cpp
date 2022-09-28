#include "ioh.hpp"

void random_search(const std::shared_ptr<ioh::problem::Integer> p)
{
    for (int i = 0; i < 100; i++)
        (*p)(ioh::common::random::integers(p->meta_data().n_variables, 0, 1));
}


void example_evaluation()
{
    using namespace ioh::problem;

    // we first create a problem factory, only for GraphProblem Types
    const auto &problem_factory = ProblemRegistry<submodular::GraphProblem>::instance();

    // We loop over all problems
    for (auto &[id, _] : problem_factory.map())
    {
        // Creating a problem instance
        // This is slower the first time the problem is created, as the graph has to be loaded from file
        // subsequent calls are faster.
        auto problem = problem_factory.create(id, 1, 1);
        std::cout << problem->meta_data() << std::endl;

        // We can then call the problem with an array of integers a would be expected
        auto x0 = ioh::common::random::integers(problem->meta_data().n_variables, 0, 1);
        std::cout << "y0: " << (*problem)(x0) << std::endl;
    }
}

void example_suite_with_logging()
{
    using namespace ioh::problem;
    
    // We can create a suite by default, this contains all submodular problems
    ioh::suite::Submodular suite;
    const int repetition = 10;

    // Logger for storing data
    auto b = ioh::logger::Analyzer({ioh::trigger::on_improvement}, {}, // no additional properties
                                   fs::current_path(), // path to store data
                                   "example_log"); // folder to store data
    suite.attach_logger(b);
    for (const auto &problem : suite)
    {
        std::cout << (*problem).meta_data() << std::endl;
        for (auto i = 0; i < repetition; i++)
        {
            random_search(problem);
            problem->reset();
        }
    }
}

int main() {
    example_suite_with_logging();
}