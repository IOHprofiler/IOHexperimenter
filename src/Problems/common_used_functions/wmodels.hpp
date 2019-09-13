/// \file wmodels.cpp
/// \brief cpp file for class w-models.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _F_WMODELS_H
#define _F_WMODELS_H

#include "../../Template/IOHprofiler_random.hpp"

static IOHprofiler_random random_methods;

static std::vector<int> dummy(int number_of_variables, double select_rate, long inseed) {
  std::vector<int> position;
  std::vector<int> random_index;
  std::vector<double> random_numbers;
  int temp;
  int select_num = (int)floor((double)(number_of_variables * select_rate));
  
  position.reserve(number_of_variables);
  for (int i = 0; i != number_of_variables; ++i) {
    position.push_back(i);
  }

  random_methods.IOHprofiler_uniform_rand((size_t)select_num,inseed,random_numbers);
  for (int i = 0; i < select_num; ++i) {
    random_index.push_back((int)floor(random_numbers[i] * 1e4 / 1e4 * number_of_variables));
  }
  for (int i = 0; i != select_num; ++i) {
    temp = position[i];
    position[i] = position[random_index[i]];
    position[random_index[i]] = temp;
  }

  /// This is a stl algorithm.
  sort(position.begin(),position.begin()+select_num);

  random_index.clear();
  for(int i = 0; i != select_num; ++i){
    random_index.push_back(position[i]);
  }
  return random_index;
}

static std::vector<int> neutrality(const std::vector<int> &variables, int mu) {
  int number_of_variables = variables.size();
  int n = (int)floor((double)number_of_variables/(double)mu);
  std::vector<int> new_variables;

  new_variables.reserve(n);
  int i = 0, temp = 0;
  while (i != number_of_variables) {
    temp += variables[i];
    if ((i+1) % mu == 0 && i != 0) {
      if (temp >= mu / 2.0) {
        new_variables.push_back(1);
      } else {
        new_variables.push_back(0);
      }
      temp = 0;
    }
    i++;
  }

  return new_variables;
}

static std::vector<int> epistasis(const std::vector<int> &variables, int v) {
  int h, epistasis_result;
  int number_of_variables = variables.size();
  std::vector<int> new_variables;
  new_variables.reserve(number_of_variables);
  h = 0;
  while (h + v - 1 < number_of_variables) {
    int i = 0;
    while(i < v) {
      epistasis_result = -1;
      for (int j = 0; j < v; ++j) {
        if ((v - j - 1) != ((v - i - 1) - 1) % 4) {
          if (epistasis_result == -1) {
            epistasis_result = variables[h+j];
          } else {
            epistasis_result = (epistasis_result != variables[h+j]);
          }
        }
      }
      new_variables.push_back(epistasis_result);
      ++i;
    }
    h += v;
  }
  if(number_of_variables - h > 0) {
    v = number_of_variables - h;
    int i = 0;
    while (i < v) {
      epistasis_result = -1;
      for (int j = 0; j < v; ++j) {
        if ((v - j - 1) != ((v - i - 1) - 1) % 4) {
          if (epistasis_result == -1) {
            epistasis_result = variables[h+j];
          } else {
            epistasis_result = (epistasis_result != variables[h+j]);
          }
        }
      }
      new_variables.push_back(epistasis_result);
      ++i;
    }
    h += v;
  }
  return new_variables;
}

static double ruggedness1(double y, int number_of_variables) {
  double ruggedness_y, s;
  s = (double)number_of_variables;
  if(y == s) {
    ruggedness_y = ceil(y/2.0) + 1.0;
  } else if (y < s && number_of_variables % 2 == 0) {
    ruggedness_y = floor(y / 2.0) + 1.0;
  } else if (y < s && number_of_variables % 2 != 0) {
    ruggedness_y = ceil(y / 2.0) + 1.0;
  } else {
    ruggedness_y = y;
    assert(y <= s);
  }
  return ruggedness_y;
}

static double ruggedness2(double y, int number_of_variables) {
  double ruggedness_y;
  int tempy=(int)(y+0.5);
  if (tempy == number_of_variables) {
    ruggedness_y = y;
  } else if (tempy < number_of_variables && tempy % 2 == 0 && number_of_variables % 2 == 0) {
    ruggedness_y = y + 1.0;
  } else if (tempy < number_of_variables && tempy % 2 == 0 && number_of_variables % 2 != 0) {
    ruggedness_y = (y - 1.0) > 0 ? (y - 1.0) : 0;
  } else if (tempy < number_of_variables && tempy % 2 != 0 && number_of_variables % 2 == 0) {
    ruggedness_y = (y - 1.0) > 0 ? (y - 1.0) : 0;
  } else if (tempy < number_of_variables && tempy % 2 != 0 && number_of_variables % 2 != 0) {
    ruggedness_y = y + 1.0;
  } else {
    ruggedness_y = y;
    assert(tempy <= number_of_variables);
  }
  return ruggedness_y;
}

static std::vector<double> ruggedness3(int number_of_variables) {
  std::vector<double> ruggedness_fitness(number_of_variables+1,0.0);
  
  for (int j = 1; j <= number_of_variables/5; ++j) {
    for (int k = 0; k < 5; ++k) {
      ruggedness_fitness[number_of_variables-5*j+k] = (double)(number_of_variables - 5 * j + (4-k));
    }
  }
  for (int k = 0; k < number_of_variables - number_of_variables / 5 * 5; ++k) {
    ruggedness_fitness[k] = (double)(number_of_variables - number_of_variables / 5 * 5 - 1 - k);
  }
  ruggedness_fitness[number_of_variables] = (double)number_of_variables;
  return ruggedness_fitness;
}

#endif
