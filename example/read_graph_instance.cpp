#include "ioh.hpp"
#include "ioh/problem/problem.hpp"


//Make sure the meta graph list file shares directory with .exe and graph data are in proper format (i.e. edge list)
//Extract example_graphs.zip there
using namespace std;

//Solver function, uses problem oracle as input
void solver(const std::shared_ptr<ioh::problem::Integer> p)
{
    //Random search
    /*for (int i = 0; i < 1000; i++)
    {
        (*p)(ioh::common::random::integers(p->meta_data().n_variables, 0, 1));
    }*/

    //Greedy
    auto x = std::vector<int>(p->meta_data().n_variables, 0);
    double cur_best = (*p)(x);
    int best_index;
    do
    {
        best_index = -1;
        for (auto i = 0; i < p->meta_data().n_variables; i++)
        {
            if (x[i] == 0)
            {
                x[i] = 1;
                double cur_value = (*p)(x);
                x[i] = 0;
                if (cur_value > cur_best)
                {
                    cur_best = cur_value;
                    best_index = i;
                }
            }
        }
        if (best_index >= 0)
            x[best_index] = 1;
    } while (best_index >= 0);
}

//Run an experiment on graph problems
//Currently, loggers don't differentiate by instance id, data from runs on different
//instances with the same dimension and problem are written into the same file.
//For now, separate loggers for different instances are used instead of suite.
//This is not ideal, clearly.
int main()
{
    // Max Vertex Cover
    ioh::problem::read_meta_list_graph(true, "example_list_maxvertexcover");
    std::vector<std::shared_ptr<ioh::problem::Integer>> problems = {
        std::make_shared<ioh::problem::pbo::MaxVertexCover>(1),
        std::make_shared<ioh::problem::pbo::MaxVertexCover>(2)
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
        std::make_shared<ioh::problem::pbo::MaxCut>(1),
        std::make_shared<ioh::problem::pbo::MaxCut>(2)
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

    // Max Influence
    ioh::problem::read_meta_list_graph(true, "example_list_maxinfluence");
    problems = std::vector<std::shared_ptr<ioh::problem::Integer>>({
        std::make_shared<ioh::problem::pbo::MaxInfluence>(1),
        std::make_shared<ioh::problem::pbo::MaxInfluence>(2)
    });
    for (auto p : problems)
    {
        auto b = ioh::logger::Analyzer({ioh::trigger::on_improvement}, {}, // no additional properties
                                       fs::current_path(), // path to store data
                                       "maxinfluence_" + std::to_string(p->meta_data().instance));
        p->attach_logger(b);
        for (auto i = 0; i < 10; i++)
        {
            solver(p);
            p->reset();
        }
    }

    // Pack While Travel
    ioh::problem::read_meta_list_graph(true, "example_list_pwt");
    problems = std::vector<std::shared_ptr<ioh::problem::Integer>>({
        std::make_shared<ioh::problem::pbo::PackWhileTravel>(1),
        std::make_shared<ioh::problem::pbo::PackWhileTravel>(2)
    });
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