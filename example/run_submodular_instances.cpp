#pragma once

#include <cmath>

#include "ioh.hpp"


//Make sure the meta list files are in the working directory and graph data are in proper format (i.e. edge list)
//Extract example_submodular.zip there

//Solver function, uses problem oracle as input
void solver(const std::shared_ptr<ioh::problem::Integer> p)
{
    //Random search
    for (int i = 0; i < 100; i++)
    {
        (*p)(ioh::common::random::integers(p->meta_data().n_variables, 0, 1));
    }

    //Greedy
    /*auto x = std::vector<int>(p->meta_data().n_variables, 0);
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
    } while (best_index >= 0);*/
}

double chebyshev_cons_factor(const double delta = 0, const double alpha = 0)
{
    return delta * sqrt((1.0 - alpha) / alpha / 3.0);
}

double chernoff_cons_factor(const double delta = 0, const double alpha = 1)
{
    return delta * sqrt(2.0 * log(1 / alpha));
}

//Run an experiment on graph problems
//Currently, loggers don't differentiate by instance id, data from runs on different
//instances with the same dimension and problem are written into the same file.
//For now, separate loggers for different instances are used instead of suite.
//This is not ideal, clearly.
int main()
{
    int repetition = 10;
    // Max Vertex Cover
    std::vector<int> instance_dimensions = ioh::problem::submodular::read_instances_from_files(
        false, "./example_list_maxcoverage");
    int instance_number = instance_dimensions.size();
    std::vector<std::shared_ptr<ioh::problem::Integer>> problems = {};
    for (auto a = 0; a < instance_number; a++)
        problems.push_back(std::make_shared<ioh::problem::submodular::MaxCoverage>(a + 1,instance_dimensions[a]));
    for (auto p : problems)
    {
        // Optional chance constraint factor
        //ioh::problem::graph_list[p->meta_data().instance - 1]->set_chance_cons_factor(chebyshev_cons_factor(0.5, 0.1));
        auto b = ioh::logger::Analyzer({ioh::trigger::on_improvement},
                                       {}, // no additional properties
                                       fs::current_path(), // path to store data
                                       "maxcoverage_"+std::to_string(p->meta_data().instance));
        p->attach_logger(b);
        for (auto i = 0; i < repetition; i++)
        {
            solver(p);
            p->reset();
        }
    }

    //Max Cut
    instance_dimensions = ioh::problem::submodular::read_instances_from_files(
        false, "./example_list_maxcut");
    instance_number = instance_dimensions.size();
    // instance_number = ioh::problem::submodular::Graph::read_meta_list_graph(
    //     true, fs::current_path().string() + "\\" + "example_list_maxcut");
    problems.clear();
    for (auto a = 0; a < instance_number; a++)
    problems.push_back(std::make_shared<ioh::problem::submodular::MaxCut>(a + 1,instance_dimensions[a]));
    for (auto p : problems)
    {
        auto b = ioh::logger::Analyzer({ioh::trigger::on_improvement},
                                       {}, // no additional properties
                                       fs::current_path(), // path to store data
                                       "maxcut_" + std::to_string(p->meta_data().instance));
        p->attach_logger(b);
        for (auto i = 0; i < repetition; i++)
        {
            solver(p);
            p->reset();
        }
    }

    // Max Influence
    instance_dimensions = ioh::problem::submodular::read_instances_from_files(
        false, "./example_list_maxinfluence");
    instance_number = instance_dimensions.size();
    // instance_number = ioh::problem::submodular::Graph::read_meta_list_graph(
    //     true, fs::current_path().string() + "\\" + "example_list_maxinfluence");
    problems.clear();
    for (auto a = 0; a < instance_number; a++)
        problems.push_back(std::make_shared<ioh::problem::submodular::MaxInfluence>(a + 1,instance_dimensions[a]));
    for (auto p : problems)
    {
        // Optional chance constraint factor
        // ioh::problem::graph_list[p->meta_data().instance - 1]->set_chance_cons_factor(chebyshev_cons_factor(0.5, 0.1));
        auto b = ioh::logger::Analyzer({ioh::trigger::on_improvement}, {}, // no additional properties
                                       fs::current_path(), // path to store data
                                       "maxinfluence_" + std::to_string(p->meta_data().instance));
        p->attach_logger(b);
        for (auto i = 0; i < repetition; i++)
        {
            solver(p);
            p->reset();
        }
    }

    // // Pack While Travel
    instance_number = ioh::problem::submodular::PackWhileTravel::read_meta_list_instance("./example_list_maxinfluence").size();
    // instance_number = ioh::problem::submodular::PackWhileTravel::read_meta_list_instance(
    //     true, fs::current_path().string() + "\\" + "example_list_pwt");
    problems.clear();
    for (auto a = 0; a < instance_number; a++)
        problems.push_back(std::make_shared<ioh::problem::submodular::PackWhileTravel>(a + 1));
    for (auto p : problems)
    {
        auto b = ioh::logger::Analyzer({ioh::trigger::on_improvement}, {}, // no additional properties
                                       fs::current_path(), // path to store data
                                       "pwt_" + std::to_string(p->meta_data().instance));
        p->attach_logger(b);
        for (auto i = 0; i < repetition; i++)
        {
            solver(p);
            p->reset();
        }
    }
}