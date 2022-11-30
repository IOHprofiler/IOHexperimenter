#include "ioh.hpp"

#include "problem_example.hpp"
#include "suite_example.hpp"
#include "logger_example.hpp"
#include "experiment_example.hpp"
#include "add_new_problem.hpp"
#include "example_submodular_instances.hpp"


int main()
{
    show_registered_objects();
    problem_example();
    suite_example();
    experiment_example();
    logger_example();
    add_problem_example();
    example_submodular_suite_with_logging();
}

