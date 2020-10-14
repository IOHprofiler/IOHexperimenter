  #include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>

#include "ioh.hpp" 
//
// #include <IOHprofiler_BBOB_suite.hpp>
// #include <IOHprofiler_problem.h>
// #include <IOHprofiler_ecdf_logger.h>



int main()
{
    // size_t sample_size = 100;
    //
    // std::vector<int> pbs = { 1,2 };
    // std::vector<int> ins = { 1,2 };
    // std::vector<int> dims = { 2,10 };
    //
    // BBOB_suite bench(pbs, ins, dims);
    // // bench.loadProblem();
    //
    // size_t ecdf_width = 20;
    // using Logger = IOHprofiler_ecdf_logger<BBOB_suite::InputType>;
    // IOHprofiler_RangeLog<double> error(0, 6e7, ecdf_width);
    // IOHprofiler_RangeLog<size_t> evals(0, sample_size, ecdf_width);
    // Logger logger(error, evals);
    //
    // logger.activate_logger();
    // logger.track_suite(bench);
    //
    // size_t seed = 5;
    // std::mt19937 gen(seed);
    // // std::mt19937 gen(time(0));
    // std::uniform_real_distribution<> dis(-5, 5);
    //
    // BBOB_suite::Problem_ptr pb;
    // size_t n = 0;
    // while ((pb = bench.get_next_problem())) {
    //     logger.track_problem(*pb);
    //
    //     std::clog << "Problem " << pb->IOHprofiler_get_problem_id()
    //         << " (" << pb->IOHprofiler_get_problem_name() << ")"
    //         << " get " << pb->IOHprofiler_get_instance_id()
    //         << ", optimum: ";
    //     for (double o : pb->IOHprofiler_get_optimal()) {
    //         std::clog << o << " ";
    //     }
    //     std::clog << std::endl;
    //
    //     size_t d = pb->IOHprofiler_get_number_of_variables();
    //     for (size_t s = 0; s < sample_size; ++s) {
    //         std::vector<double> sol;
    //         sol.reserve(d);
    //         std::generate_n(std::back_inserter(sol), d, [&dis, &gen]() {return dis(gen); });
    //
    //         double f = pb->evaluate(sol);
    //         logger.do_log(pb->loggerInfo());
    //     }
    //
    //     n++;
    // } // for name_id
    // std::clog << "Done " << n << " function test" << std::endl;
    //
    // size_t i, j, k;
    // std::tie(i, j, k) = logger.size();
    // std::clog << i << " problems × " << j << " dimensions × " << k << " instances" << std::endl;
    // assert(i == pbs.size());
    // assert(j == dims.size());
    // assert(k == ins.size());
    //
    // for (int ipb : pbs) {
    //     for (int idim : dims) {
    //         for (int iins : ins) {
    //             std::clog << "Problem " << ipb
    //                 << ", dimension " << idim
    //                 << ", get " << iins
    //                 << ": " << std::endl;
    //             const auto& m = logger.at(ipb, iins, idim);
    //             std::clog << m << std::endl;
    //             assert(m.size() == ecdf_width);
    //             assert(m[0].size() == ecdf_width);
    //         }
    //     }
    // }
    //
    // IOHprofiler_ecdf_sum sum;
    // size_t s = sum(logger.data());
    // std::clog << "Attainments sum: ";
    // std::cout << s << std::endl;
    // assert(s <= sample_size * ecdf_width * ecdf_width * i * j * k);
}
