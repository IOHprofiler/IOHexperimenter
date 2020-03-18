#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>

#include <IOHprofiler_BBOB_suite.hpp>
#include <IOHprofiler_problem.hpp>
#include <IOHprofiler_csv_logger.h>

int main()
{
    size_t sample_size = 3;
    size_t runs = 3;

    BBOB_suite bench;
    // bench.loadProblem();
    auto bench_problems = bench.IOHprofiler_suite_get_problems();

    IOHprofiler_csv_logger ioh_logger(
            /*output_directory*/"ioh_data",
            /*folder_name*/"experiment",
            "solver",
            "Random search");
    try {
        ioh_logger.activate_logger(); // Check if result directory is writable.
    } catch(std::runtime_error& e) {
        std::cerr << "ERROR in IOH logger preconditions: " << e.what() << std::endl;
        exit(1);
    }
    ioh_logger.set_complete_flag(true); // Dump everything at every function call in a *.cdat file.


    size_t seed = 1;
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> dis(-5,5);

    BBOB_suite::Problem_ptr pb;
    size_t n=0;
    while(pb = bench.get_next_problem()) {
        for(size_t i=0; i < runs; ++i) {
            std::clog << "[";
            std::clog.flush();
            auto d = pb->IOHprofiler_get_number_of_variables();
            ioh_logger.target_problem(*pb);

            std::vector<double> lowerbounds = pb->IOHprofiler_get_lowerbound();
            std::vector<double> upperbounds = pb->IOHprofiler_get_upperbound();

            for(size_t s=0; s < sample_size; ++s) {
                std::clog << ".";
                std::clog.flush();

                std::vector<double> sol;
                sol.reserve(d);
                std::generate_n(std::back_inserter(sol), d, [&dis,&gen](){return dis(gen);});

                double f = pb->evaluate(sol);
                ioh_logger.write_line(pb->loggerInfo());
            }

            n++;
            std::clog << "]";
            std::clog.flush();
        }
        std::clog << std::endl;
    } // for name_id

    std::clog << "Done " << n << " function test" << std::endl;
}
