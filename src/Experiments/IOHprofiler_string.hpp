#ifndef _IOHPROFILER_STRING_HPP
#define _IOHPROFILER_STRING_HPP

// #include <string>
// #include <vector>
 #include <sstream>
#include "../common.h"

std::string strstrip(std::string s)
{
  if (s.empty()) 
  {
      return s;
  }
  s.erase(0,s.find_first_not_of(" "));
  s.erase(s.find_last_not_of(" ") + 1);
  return s;
};



std::vector<int> get_int_vector_parse_string(std::string input, const int _min, const int _max) {
  std::vector<std::string> spiltstring;
  std::string tmp;
  int tmpvalue,tmpvalue1;
  std::vector<int> result;

  size_t n = input.size();
  input = strstrip(input);
  for(size_t i = 0; i < n; ++i) {
    if(input[i] != ',' && input[i] != '-' && !isdigit(input[i]))
      IOH_error("The configuration consists invalid characters.");
  }
  
  std::stringstream raw(input);
  while(getline(raw, tmp, ',')) spiltstring.push_back(tmp);

  n = spiltstring.size();
  for (size_t i = 0; i < n; ++i) {
    size_t l = spiltstring[i].size();

    // The condition beginning with "-m"
    if(spiltstring[i][0] == '-') {
      if(i != 0) {
        IOH_error("Format error in configuration.");
      }else {
        tmp = spiltstring[i].substr(1);
        if(tmp.find('-') != std::string::npos) IOH_error("Format error in configuration.");
        tmpvalue = std::stoi(tmp);
        if(tmpvalue < _min) IOH_error("Input value exceeds lowerbound.");
        for(int value = _min; value <= tmpvalue; ++value) result.push_back(value);
      }
    }

    // The condition endding with "n-"
    else if(spiltstring[i][spiltstring[i].length()-1] == '-') {
      if(i != spiltstring.size() - 1) {
        IOH_error("Format error in configuration.");
      }else {
        tmp = spiltstring[i].substr(0,spiltstring[i].length() - 1);
        if(tmp.find('-') != std::string::npos) IOH_error("Format error in configuration.");
        tmpvalue = std::stoi(tmp);
        if(tmpvalue > _max) IOH_error("Input value exceeds upperbound.");
        for(int value = _max; value <= tmpvalue; --value) result.push_back(value);
      }
    }

    //The condition with "n-m,n-x-m"
    else {
      std::stringstream tempraw(spiltstring[i]);
      std::vector<std::string> tmpvaluevector;
      while(getline(tempraw, tmp, '-')) tmpvaluevector.push_back(tmp);
      tmpvalue = std::stoi(tmpvaluevector[0]);
      tmpvalue1 = std::stoi(tmpvaluevector[tmpvaluevector.size()-1]);
      if(tmpvalue > tmpvalue1) IOH_error("Format error in configuration.");
      if(tmpvalue < _min)  IOH_error("Input value exceeds lowerbound.");
      if(tmpvalue1 > _max) IOH_error("Input value exceeds upperbound.");
      for(int value = tmpvalue; value <= tmpvalue1; ++value) result.push_back(value);
    } 
  }
  return result;
};

#endif