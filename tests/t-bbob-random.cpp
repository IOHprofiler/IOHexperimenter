#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>

#include <IOHprofiler_BBOB_suite.hpp>
#include <IOHprofiler_problem.h>

int main()
{
    size_t sample_size = 10;

    BBOB_suite bench;
    // bench.loadProblem();
    auto bench_problems = bench.IOHprofiler_suite_get_problems();
    assert(bench_problems.size() == 24);

    size_t seed = 1;
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> dis(-5,5);

    BBOB_suite::Problem_ptr pb;
    size_t n=0;
    while((pb = bench.get_next_problem())) {
        std::clog << "[";
        std::clog.flush();
        auto d = pb->IOHprofiler_get_number_of_variables();

        assert(pb->IOHprofiler_get_number_of_objectives() == 1);
        assert(pb->IOHprofiler_get_optimization_type() == IOH_optimization_type::Minimization);

        std::vector<double> lowerbounds = pb->IOHprofiler_get_lowerbound();
        std::vector<double> upperbounds = pb->IOHprofiler_get_upperbound();
        assert(lowerbounds.size() == upperbounds.size());
        assert(lowerbounds.size() > 0);

        for(size_t i=0; i < lowerbounds.size(); ++i) {
            assert(lowerbounds.at(i) = -5);
            assert(upperbounds.at(i) =  5);
        }

        for(size_t s=0; s < sample_size; ++s) {
            std::clog << ".";
            std::clog.flush();

            std::vector<double> sol;
            sol.reserve(d);
            std::generate_n(std::back_inserter(sol), d, [&dis,&gen](){return dis(gen);});

            double f = pb->evaluate(sol);
            assert(std::isfinite(f));
        }

        n++;
        std::clog << "]" << std::endl;
        std::clog.flush();
    } // for name_id

    std::clog << "Done " << n << " function test" << std::endl;
}
