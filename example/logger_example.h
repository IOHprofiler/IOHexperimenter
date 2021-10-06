#pragma once

#include "ioh.hpp" 

inline ioh::logger::Analyzer get_logger(const std::string &folder_name = "logger_example", const bool store_positions = false)
{
    /// Instantiate a logger.
    using namespace ioh;
    return logger::Analyzer(
        {trigger::on_improvement}, // trigger when the objective value improves
        {},                        // no additional properties 
        fs::current_path(),        // path to store data
        folder_name,               // name of the folder in path, which will be newly created
        "PSO",                     // name of the algoritm 
        "Type1",                   // additional info about the algorithm              
        store_positions            // where to store x positions in the data files 
    );
}


/// An example of using the default logger class to store evaluation information during the optimization process.
inline void single_logger_example()
{
    std::cout << "==========\nAn example of testing logger\n==========" << std::endl;
    auto logger = get_logger();
    /// Instatiate a bbob suite of problem {1,2}, intance {1, 2} and dimension {5,10}.
    const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();
    const auto suite = suite_factory.create("BBOB", {1, 2}, {1, 2}, {5, 10});


    /// Show output folder of the logger 
    std::cout << "Storing data at: " << logger.output_directory() << std::endl; 

    
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
        for (auto budget = 100; budget > 0; budget--)
        {
            /// Evaluate the fitness of 'x' for the problem by using '(*problem)(x)' function.
            (*problem)(ioh::common::random::doubles(problem->meta_data().n_variables, -5, 5));
        }
        std::cout << "result: " << problem->state() << std::endl;
    }
}

/// Example of combining multiple loggers for a given experiment
inline void logger_combiner_example()
{
    std::cout << "==========\nAn example of logger combiner \n==========" << std::endl;
    auto logger1 = get_logger("logger_wo_positions");
    auto logger2 = get_logger("logger_with_positions", true);
    auto logger = ioh::logger::Combine({logger1, logger2});
    
    auto problem = ioh::problem::pbo::OneMax(1, 10);
    // We can use a LoggerCombine object as a 'normal' logger object
    problem.attach_logger(logger);
    problem(ioh::common::random::integers(10, 0, 1));
}


inline void logger_with_custom_parameters_example()
{
    
    std::cout << "==========\nAn example of logger variables \n==========" << std::endl;

    auto logger = get_logger("logger_w_custom_parameters");
    auto problem = ioh::problem::bbob::Sphere(1, 3);
    
    // Already intialize these parameters, so we can access the pointer
    double run_id = 1.0;
    std::vector<double> x(problem.meta_data().n_variables);
    
    // Add parameters fixed throughout the experiment.
    logger.add_experiment_attribute("meta_data_x", "69");
    logger.add_experiment_attribute("meta_data_y", "69");


    // Declare parameters unique for each run. 
    logger.add_run_attribute("run_id", &run_id);
    
    // Add additional parameters, stored as columns in the data 
    logger.watch(ioh::watch::address("x0", x.data()));
    logger.watch(ioh::watch::address("x1", x.data() + 1));

    problem.attach_logger(logger);

    // Run a simple experiment
    for (run_id = 1; run_id < 2; run_id++)
    {
        for (auto i = 0; i < 10; i ++)
        {
            std::generate(x.begin(), x.end(), [](){return ioh::common::random::real(-1, 1);});
            problem(x);
        }
        problem.reset();
    }
}


inline void logger_example()
{
    single_logger_example();
    logger_combiner_example();
    logger_with_custom_parameters_example();
}
