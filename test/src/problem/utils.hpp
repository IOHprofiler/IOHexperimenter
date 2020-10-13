#pragma once

#include <vector>
#include <fstream>
#include <string>

using namespace std;

static vector<string> split(const string &str, const string &pattern)
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
 
static vector<double> string_to_vector_double(string s) {
  vector<double> x;
  vector<string> tmp;
  int i = 0;

  tmp = split(s,",");
  while(i != tmp.size()) {
    x.push_back( stod(tmp[i]) );
    i++;
  }
  return x;
}

static vector<int> string_to_vector_int(string s) {
    vector<int> x;
    int i = 0;
    while (i != s.size()) {
        x.push_back(s[i] - '0');
        i++;
    }
    return x;
}

