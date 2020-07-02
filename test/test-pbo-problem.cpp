#include <vector>
#include <fstream>
#include <string>
#include "IOHprofiler_PBO_suite.hpp"
#include "gtest/gtest.h"

using namespace std;

vector<int> stringToVectorInt(string s) {
  vector<int> x;
  int i = 0;
  while(i != s.size()) {
    x.push_back(s[i] - '0');
    i++;
  }
  return x;
}

vector<string> split(const string &str, const string &pattern)
{
    vector<string> res;
    if(str == "")
        return res;

    string strs = str + pattern;
    size_t pos = strs.find(pattern);

    while(pos != strs.npos)
    {
        string temp = strs.substr(0, pos);
        res.push_back(temp);
        strs = strs.substr(pos+1, strs.size());
        pos = strs.find(pattern);
    }
    return res;
}

TEST(PBOfitness, dimension16) {
  string file_name = "./pbofitness16.in";
  std::string s;
  vector<string> tmp;
  int funcId;
  int insId;
  std::vector<int> x;
  double y,f;

  PBO_suite pbo;
  std::shared_ptr<IOHprofiler_problem<int>> problem;


  ifstream infile(file_name.c_str());
    while( getline(infile,s) ) {
      tmp = split(s," ");
      funcId = stoi(tmp[0]);
      insId = stoi(tmp[1]);
      x =  stringToVectorInt(tmp[2]);
      f = stod(tmp[3]);
      
      problem = pbo.get_problem(funcId,insId,16);
      y = problem->evaluate(x);
      EXPECT_LE(abs(f - y), 0.0001) << "The fitness of function " << funcId << "( ins " << insId << " ) is " <<  f << " ( not " << y << ").";
    }
}

TEST(PBOfitness, dimension100) {
  string file_name = "./pbofitness100.in";
  std::string s;
  vector<string> tmp;
  int funcId;
  int insId;
  std::vector<int> x;
  double y,f;

  PBO_suite pbo;
  std::shared_ptr<IOHprofiler_problem<int>> problem;


  ifstream infile(file_name.c_str());
    while( getline(infile,s) ) {
      tmp = split(s," ");
      funcId = stoi(tmp[0]);
      insId = stoi(tmp[1]);
      x =  stringToVectorInt(tmp[2]);
      f = stod(tmp[3]);
      
      problem = pbo.get_problem(funcId,insId,100);
      y = problem->evaluate(x);
      EXPECT_LE(abs(f - y), 0.0001) << "The fitness of function " << funcId << "( ins " << insId<< " ) is " << f << " ( not " << y << ").";
    }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}