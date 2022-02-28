#include "ioh.hpp"
#include "ioh/problem/problem.hpp"


//Make sure the meta graph list file shares directory with .exe and graph data are in proper format (i.e. edge list)
//Extract example_graphs.zip there
using namespace std;

//Random search
void solver(const std::shared_ptr<ioh::problem::Integer> p)
{
    for (int i = 0; i < 1000; i++)
    {
        (*p)(ioh::common::random::integers(p->meta_data().n_variables, 0, 1));
    }
}

//Run an experiment on graph problems
int main()
{
    const auto b = std::make_shared<ioh::logger::Analyzer>();
    const auto suite =
        ioh::suite::SuiteRegistry<ioh::problem::Integer>::instance().create("PBO", {101, 102}, {1, 3}, {1});
    ioh::Experimenter<ioh::problem::Integer> f(suite, b, solver, 10);
    f.run();
}