#include <vector>
#include <fstream>
#include <string>

#include <gtest/gtest.h>
#include "ioh.hpp"
#include "utils.hpp"


using namespace std;

TEST(PBOfitness, dimension16) {
  string file_name = "./pbofitness16.in";
  std::string s;
  vector<string> tmp;
  int funcId;
  int insId;
  std::vector<int> x;
  double y,f;

  ioh::suite::pbo pbo;
  std::shared_ptr<ioh::problem::pbo::pbo_base> problem;


  ifstream infile(file_name.c_str());
    while( getline(infile,s) ) {
      tmp = split(s," ");
      funcId = stoi(tmp[0]);
      insId = stoi(tmp[1]);
      x =  string_to_vector_int(tmp[2]);
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

  ioh::suite::pbo pbo;
  std::shared_ptr<ioh::problem::pbo::pbo_base> problem;


  ifstream infile(file_name.c_str());
    while( getline(infile,s) ) {
      tmp = split(s," ");
      funcId = stoi(tmp[0]);
      insId = stoi(tmp[1]);
      x = string_to_vector_int(tmp[2]);
      f = stod(tmp[3]);
      
      problem = pbo.get_problem(funcId,insId,100);
      y = problem->evaluate(x);
      EXPECT_LE(abs(f - y), 0.0001) << "The fitness of function " << funcId << "( ins " << insId<< " ) is " << f << " ( not " << y << ").";
    }
}