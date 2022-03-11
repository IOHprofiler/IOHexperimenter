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
//Currently, loggers don't differentiate by instance id, data from runs on different
//instances of the same dimension and problem are written into the same file.
//For now, separate loggers for different instances are used instead of suite.
//This is not ideal, clearly.
int main()
{
    // Max Vertex Cover
    ioh::problem::read_meta_list_graph(true, "example_list_maxvertexcover");
    std::vector<std::shared_ptr<ioh::problem::Integer>> problems = {
        std::make_shared<ioh::problem::pbo::MaxVertexCover>(1), std::make_shared<ioh::problem::pbo::MaxVertexCover>(2)
    };
    for (auto p : problems)
    {
        auto b = ioh::logger::Analyzer({ioh::trigger::on_improvement},
                                       {}, // no additional properties
                                       fs::current_path(), // path to store data
                                       "maxvertex_"+std::to_string(p->meta_data().instance));
        p->attach_logger(b);
        for (auto i = 0; i < 10; i++)
        {
            solver(p);
            p->reset();
        }
    }

    // Max Cut
    ioh::problem::read_meta_list_graph(true, "example_list_maxcut");
    problems = std::vector<std::shared_ptr<ioh::problem::Integer>>({
        std::make_shared<ioh::problem::pbo::MaxCut>(1), std::make_shared<ioh::problem::pbo::MaxCut>(2)
    });
    for (auto p : problems)
    {
        auto b = ioh::logger::Analyzer({ioh::trigger::on_improvement},
                                       {}, // no additional properties
                                       fs::current_path(), // path to store data
                                       "maxcut_" + std::to_string(p->meta_data().instance));
        p->attach_logger(b);
        for (auto i = 0; i < 10; i++)
        {
            solver(p);
            p->reset();
        }
    }

    // Pack While Travel
    ioh::problem::read_meta_list_graph(true, "example_list_pwt");
    problems = std::vector<std::shared_ptr<ioh::problem::Integer>>({std::make_shared<ioh::problem::pbo::PackWhileTravel>(1),
                                                             std::make_shared<ioh::problem::pbo::PackWhileTravel>(2)});
    for (auto p : problems)
    {
        auto b = ioh::logger::Analyzer({ioh::trigger::on_improvement}, {}, // no additional properties
                                       fs::current_path(), // path to store data
                                       "pwt_" + std::to_string(p->meta_data().instance));
        p->attach_logger(b);
        for (auto i = 0; i < 10; i++)
        {
            solver(p);
            p->reset();
        }
    }
}