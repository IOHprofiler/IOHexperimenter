// Author: Viet Anh Do

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

// Run an experiment on graph problems
int main()
{
    int repetition = 10;
    std::vector<std::shared_ptr<ioh::problem::Integer>> problems = {};
    std::cout << "Current working dir: " << fs::current_path().string() << std::endl;

    std::vector<std::string> instance_list_paths({
        fs::current_path().string() + "/example_list_maxcoverage",
        fs::current_path().string() + "/example_list_maxinfluence",
        fs::current_path().string() + "/example_list_maxcut",
        fs::current_path().string() + "/example_list_pwt"
        });

    // Max Vertex Cover
    // Call this function to overwrite default path to instance list file, otherwise specify it in problem constructor
    // The latter is recommended if different problems are to be run in parallel (not overwriting default path)
    int instance_number = ioh::problem::submodular::Helper::read_list_instance(instance_list_paths[0]).size();
    for (auto a = 1; a <= instance_number; a++)
    {
        // To specify path: std::make_shared<ioh::problem::submodular::MaxCoverage>("path_to_list",a)
        auto problem = std::make_shared<ioh::problem::submodular::MaxCoverage>(a);
        if (!problem->is_null())
            problems.push_back(problem);
    }

    // Max Influence
    // Call this function to override default path to instance list file, otherwise specify it in problem constructor
    instance_number = ioh::problem::submodular::Helper::read_list_instance(instance_list_paths[1]).size();
    for (auto a = 1; a <= instance_number; a++)
    {
        auto problem = std::make_shared<ioh::problem::submodular::MaxInfluence>(a);
        if (!problem->is_null())
            problems.push_back(problem);
    }

    // Max Cut
    // Call this function to override default path to instance list file, otherwise specify it in problem constructor
    instance_number = ioh::problem::submodular::Helper::read_list_instance(instance_list_paths[2]).size();
    for (auto a = 1; a <= instance_number; a++)
    {
        auto problem = std::make_shared<ioh::problem::submodular::MaxCut>(a);
        if (!problem->is_null())
            problems.push_back(problem);
    }

    // Pack While Travel
    // Call this function to override default path to instance list file, otherwise specify it in problem constructor
    instance_number = ioh::problem::submodular::Helper::read_list_instance(instance_list_paths[3]).size();
    for (auto a = 1; a <= instance_number; a++)
    {
        auto problem = std::make_shared<ioh::problem::submodular::PackWhileTravel>(a);
        if (!problem->is_null())
            problems.push_back(problem);
    }

    auto b = ioh::logger::Analyzer({ioh::trigger::on_improvement}, {}, // no additional properties
                                    fs::current_path(), // path to store data
                                   "example_log"); // folder to store data
    for (auto p : problems)
    {
        // Some problem properties
        /*std::cout << "Problem size: " << std::to_string(p->meta_data().n_variables)
            << " Constraint size: " << std::to_string(p->constraint().lb.size())
            << std::endl;*/
        p->attach_logger(b);
        for (auto i = 0; i < repetition; i++)
        {
            solver(p);
            p->reset();
        }
        p->detach_logger();
    }
}