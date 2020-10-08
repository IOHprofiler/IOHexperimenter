#include <vector>
#include <fstream>
#include <string>
#include <gtest/gtest.h>

#include "ioh.hpp"

using namespace std;


static vector<string> split(const string& str, const string& pattern)
{
    vector<string> res;
    if (str == "")
        return res;

    string strs = str + pattern;
    size_t pos = strs.find(pattern);

    while (pos != strs.npos)
    {
        string temp = strs.substr(0, pos);
        res.push_back(temp);
        strs = strs.substr(pos + 1, strs.size());
        pos = strs.find(pattern);
    }
    return res;
}

static vector<double> stringToVectorDouble(string s) {
    vector<double> x;
    vector<string> tmp;
    int i = 0;

    tmp = split(s, ",");
    while (i != tmp.size()) {
        x.push_back(stod(tmp[i]));
        i++;
    }
    return x;
}


TEST(BBOBfitness, dimension5) {
    string file_name = "./bbobfitness5.in";
    std::string s;
    vector<string> tmp;
    int funcId;
    int insId;
    std::vector<double> x;
    double y, f;

    ioh::suite::bbob bbob;
    bbob.loadProblem();
    std::shared_ptr<ioh::problem::bbob::bbob_base> problem;


    ifstream infile(file_name.c_str());
    while (getline(infile, s)) {
        tmp = split(s, " ");
        funcId = stoi(tmp[0]);
        insId = stoi(tmp[1]);
        x = stringToVectorDouble(tmp[2]);
        f = stod(tmp[3]);

        problem = bbob.get_problem(funcId, insId, 5);
        y = problem->evaluate(x);
        EXPECT_LE(abs(y - f) / f, 1.0 / pow(10, 6 - log(10))) << "The fitness of function " << funcId << "( ins " << insId << " ) is " << f << " ( not " << y << ").";
    }
}

TEST(BBOBfitness, dimension20) {
    string file_name = "./bbobfitness20.in";
    std::string s;
    vector<string> tmp;
    int funcId;
    int insId;
    std::vector<double> x;
    double y, f;

    ioh::suite::bbob bbob;
    std::shared_ptr<ioh::problem::bbob::bbob_base> problem;


    ifstream infile(file_name.c_str());
    while (getline(infile, s)) {
        tmp = split(s, " ");
        funcId = stoi(tmp[0]);
        insId = stoi(tmp[1]);
        x = stringToVectorDouble(tmp[2]);
        f = stod(tmp[3]);

        problem = bbob.get_problem(funcId, insId, 20);
        y = problem->evaluate(x);
        EXPECT_LE(abs(y - f) / f, 1.0 / pow(10, 6 - log(10))) << "The fitness of function " << funcId << "( ins " << insId << " ) is " << f << " ( not " << y << ").";
    }
}

