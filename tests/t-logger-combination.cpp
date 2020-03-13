#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>

#include <IOHprofiler_BBOB_suite.hpp>
#include <IOHprofiler_problem.h>
#include <IOHprofiler_ecdf_logger.h>
#include <IOHprofiler_csv_logger.h>
#include <IOHprofiler_observer_combine.h>

int main()
{
    size_t sample_size = 100;

    std::vector<int> pbs = {1,2};
    std::vector<int> ins = {1,2};
    std::vector<int> dims = {2,10};

    BBOB_suite bench(pbs,ins,dims);
    using Input = BBOB_suite::InputType;
    // bench.loadProblem();

    IOHprofiler_ecdf_logger<Input> log_ecdf(0,6e7,20, 0,sample_size,20);
    IOHprofiler_csv_logger<Input> log_csv; // Use default arguments.
    log_csv.activate_logger(); // FIXME only in CSV logger, should be called by constructor?

    IOHprofiler_observer_combine<Input> loggers({&log_ecdf,&log_csv});

    loggers.track_suite(bench);

    size_t seed = 5;
    std::mt19937 gen(seed);
    // std::mt19937 gen(time(0));
    std::uniform_real_distribution<> dis(-5,5);

    BBOB_suite::Problem_ptr pb;
    size_t n=0;
    while((pb = bench.get_next_problem())) {
        loggers.track_problem(*pb);

        std::clog << "Problem " << pb->IOHprofiler_get_problem_id()
                  << " (" << pb->IOHprofiler_get_problem_name() << ")"
                  << " instance " << pb->IOHprofiler_get_instance_id()
                  << ", optimum: ";
        for(double o : pb->IOHprofiler_get_optimal()) {
            std::clog << o << " ";
        }
        std::clog << std::endl;

        size_t d = pb->IOHprofiler_get_number_of_variables();
        for(size_t s=0; s < sample_size; ++s) {
            std::vector<double> sol;
            sol.reserve(d);
            std::generate_n(std::back_inserter(sol), d, [&dis,&gen](){return dis(gen);});

            double f = pb->evaluate(sol);
            loggers.do_log(pb->loggerInfo());
        }

        n++;
    } // for name_id
    std::clog << "Done " << n << " function test" << std::endl;
}
