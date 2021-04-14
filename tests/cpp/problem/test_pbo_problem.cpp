#include <vector>
#include <fstream>
#include <string>

#include <gtest/gtest.h>
#include "ioh.hpp"
#include "utils.hpp"


using namespace std;

TEST(PBOfitness, dimension16)
{
    ioh::common::log::log_level = ioh::common::log::Level::Warning;
    const string file_name = "cpp/problem/pbofitness16.in";
    std::string s;
    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::PBO>::instance();

    ifstream infile(file_name.c_str());
    while (getline(infile, s))
    {
        auto tmp = split(s, " ");
        auto func_id = stoi(tmp[0]);
        auto ins_id = stoi(tmp[1]);
        auto x = string_to_vector_int(tmp[2]);
        auto f = stod(tmp[3]);

        auto problem = problem_factory.create(func_id, ins_id, 16);
        auto y = (*problem)(x).at(0);

        EXPECT_LE(abs(f - y), 0.0001) << "The fitness of function " << func_id << "( ins " << ins_id << " ) is " << f <<
 " ( not " << y << ").";
    }
}

TEST(PBOfitness, dimension100)
{
    const string file_name = "cpp/problem/pbofitness100.in";
    std::string s;
    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::PBO>::instance();
    ifstream infile(file_name.c_str());
    while (getline(infile, s))
    {
        auto tmp = split(s, " ");
        auto func_id = stoi(tmp[0]);
        auto ins_id = stoi(tmp[1]);
        auto x = string_to_vector_int(tmp[2]);
        auto f = stod(tmp[3]);

        auto problem = problem_factory.create(func_id, ins_id, 100);
        auto y = (*problem)(x).at(0);
        EXPECT_LE(abs(f - y), 0.0001) << "The fitness of function " << func_id << "( ins " << ins_id << " ) is " << f <<
 " ( not " << y << ").";
    }
}
