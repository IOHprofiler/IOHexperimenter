#include <vector>
#include <fstream>
#include <string>
#include "ioh.hpp"
#include <gtest/gtest.h>

#include "utils.hpp"

using namespace std;

TEST(BBOBfitness, dimension5)
{
    ioh::common::log::log_level = ioh::common::log::Level::Warning;
    const string file_name = "cpp/problem/bbobfitness5.in";
    std::string s;

    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::BBOB>::instance();

    ifstream infile(file_name.c_str());
    while (getline(infile, s))
    {
        auto tmp = split(s, " ");
        auto func_id = stoi(tmp[0]);
        auto ins_id = stoi(tmp[1]);
        auto x = string_to_vector_double(tmp[2]);
        auto f = stod(tmp[3]);
        auto problem = problem_factory.create(func_id, ins_id, 5);
        auto y = (*problem)(x).at(0);
        EXPECT_LE(abs(y - f) / f, 1.0/pow(10,6-log(10))) << "The fitness of function " << func_id << "( ins " << ins_id
 << " ) is " << f << " ( not " << y << ").";
    }
}

TEST(BBOBfitness, dimension20)
{
    ioh::common::log::log_level = ioh::common::log::Level::Warning;
    const string file_name = "cpp/problem/bbobfitness20.in";
    std::string s;

    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::BBOB>::instance();

    ifstream infile(file_name.c_str());
    while (getline(infile, s))
    {
        auto tmp = split(s, " ");
        auto func_id = stoi(tmp[0]);
        auto ins_id = stoi(tmp[1]);
        auto x = string_to_vector_double(tmp[2]);
        auto f = stod(tmp[3]);

        auto problem = problem_factory.create(func_id, ins_id, 20);
        auto y = (*problem)(x).at(0);
        EXPECT_LE(abs(y - f) / f, 1.0 / pow(10, 6 - log(10))) << "The fitness of function " << func_id << "( ins " <<
 ins_id << " ) is " << f << " ( not " << y << ").";
    }
}
