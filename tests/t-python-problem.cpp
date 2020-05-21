#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>

#include <IOHprofiler_extern_python.h>
#include <IOHprofiler_problem.h>

int main(int argc, char** argv)
{
    assert(argc == 3);

    IOHprofiler_ExternPythonProblem<int> pypb(argv[1], argv[2]);

    size_t d = pypb.IOHprofiler_get_number_of_variables();
    std::vector<int> sol(d,6);

    double fit = pypb.evaluate(sol);
    std::cout << fit << std::endl;
}
