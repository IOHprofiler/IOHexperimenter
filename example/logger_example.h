#pragma once

#include "ioh.hpp"

inline ioh::logger::Default get_logger(const std::string &folder_name = "logger_example")
{
    /// Instantiate a logger.
    return ioh::logger::Default(
        fs::current_path(), /* output_directory : fs::current_path() -> working directory */
        folder_name, /* folder_name */
        "random_search", /* algorithm_name : "random search'*/
        "a random search" /* algorithm_info : "a random search for testing the bbob suite" */
        );
}


inline ioh::logger::Default get_logger_with_positions()
{
    /// Instantiate a logger that also stores evaluated search points
    return ioh::logger::Default(
        fs::current_path(), /* output_directory : fs::current_path() -> working directory */
        "logger_w_positions", /* folder_name : "logger_example" */
        "random_search", /* algorithm_name : "random search'*/
        "a random search", /* algorithm_info : "a random search for testing the bbob suite" */
        true /* store_positions: true*/
        );
}


/// An example of using the default logger class to store evaluation information during the optimization process.
inline void single_logger_example()
{
    auto logger = get_logger();
    /// Instatiate a bbob suite of problem {1,2}, intance {1, 2} and dimension {5,10}.
    const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();
    const auto suite = suite_factory.create("BBOB", {1, 2}, {1, 2}, {5, 10});

    std::cout << "==========\nAn example of testing logger\n==========" << std::endl;

    /// Show output folder of the logger 
    std::cout << "Storing data at: " << logger.experiment_folder() << std::endl; 

    
    /// Attach logger to a suite. We can either attach a logger to a suite or for
    /// each new problem in a suite manually (e.g. call problem->attach_logger(logger)
    /// for each new problem), the functionality is the same. 
    suite->attach_logger(logger);

    /// To access problems of the suite.
    for (const auto &problem : *suite)
    {
        /// Output information of the current problem.
        std::cout << problem->meta_data() << std::endl;

        /// Random search on the problem with the given budget 100.
        std::vector<double> x(problem->meta_data().n_variables);
        for (auto budget = 100; budget > 0; budget--)
        {
            ioh::common::Random::uniform(x.size(), budget, x);
            for (auto &xi : x)
                xi = xi * 10 - 5;

            /// Evaluate the fitness of 'x' for the problem by using '(*problem)(x)' function.
            (*problem)(x);
        }
        std::cout << "result: " << problem->state() << std::endl;
    }
}

/// Example of combining multiple loggers for a given experiment
inline void logger_combiner_example()
{
    auto logger1 = get_logger("logger_wo_positions");
    auto logger2 = get_logger_with_positions();
    auto logger = ioh::logger::LoggerCombine({&logger1, &logger2});

    auto problem = ioh::problem::pbo::OneMax(1, 10);
    // We can use a LoggerCombine object as a 'normal' logger object
    problem.attach_logger(logger);
    problem(ioh::common::Random::integers(10, 0, 1));
}


inline void logger_with_custom_parameters_example()
{
    auto logger = get_logger("logger_w_custom_parameters");
    auto problem = ioh::problem::bbob::Sphere(1, 3);

    // Add parameters fixed throughout the experiment.
    logger.add_experiment_attribute("meta_data_x", 69);
    logger.add_experiment_attribute("meta_data_y", 69);

    // Initialize parameters unique for each run. 
    logger.create_run_attributes({"run_id"});

    // Initialize parameters unique for each evaluation.
    logger.create_logged_attributes({"x1"});

    // Run a simple experiment
    for (auto run_id = 1; run_id < 2; run_id++)
    {
        // Update the variable for the run specific parameter
        logger.set_run_attributes({"run_id"}, {static_cast<double>(run_id)});
        for (auto i = 0; i < 10; i ++)
        {
            const auto x = ioh::common::Random::uniform(problem.meta_data().n_variables);
            // Update the variable for the evaluation specific parameter
            logger.set_logged_attributes({"x1"}, {x.at(1)});
            problem(x);
        }
    }
}


inline void logger_example()
{
    single_logger_example();
    logger_combiner_example();
    logger_with_custom_parameters_example();
}
