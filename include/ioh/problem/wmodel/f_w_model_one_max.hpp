#pragma once
#include "wmodel_problem.hpp"

namespace ioh
{
  namespace problem
  {
    namespace wmodel
    {
      class W_Model_OneMax : public WModelProblem<W_Model_OneMax>
      {
      protected:
        std::vector<double> evaluate(const std::vector<int> &x) override
        {
          std::vector<int> w_model_x;
          std::vector<int> tempX;
          int n;
          
          // Dummy Layer
          if (this->dummy_para > 0)
          {
            n = static_cast<int>(this->dummy_info.size());
            w_model_x.reserve(n);
            for (auto i = 0; i != n; ++i)
            {
              w_model_x.push_back(x[this->dummy_info[i]]);
            }
          } else
          {
            w_model_x = x;
          }
          
          // Neutrality layer
          if (this->neutrality_para > 0)
          {
            tempX = w_model_x;
            utils::layer_neutrality_compute(tempX, w_model_x, this->neutrality_para);
          }
          
          // Epistasis layer
          if (this->epistasis_para > 0)
          {
            tempX = w_model_x;
            utils::layer_epistasis_compute(tempX, w_model_x, this->epistasis_para);
          }
          
          // Base evaluate
          n = static_cast<int>(w_model_x.size());
          auto result = std::accumulate(w_model_x.begin(), w_model_x.end(), 0.0);
          
          // Ruggedness layer
          if (this->ruggedness_para > 0)
          {
            result = this->ruggedness_info[result];
          }
          
          return {static_cast<double>(result)};
        }
        
      public:
        W_Model_OneMax(const int instance, const int n_variables) :
        WModelProblem(1, instance, n_variables, "W_Model_OneMax", 0, 0, 0, 0)
        {
        }
        
        W_Model_OneMax(const int instance, const int n_variables,
                            const double dummy_para, const int epistasis_para, const int neutrality_para, const double ruggedness_para) :
        WModelProblem(1, instance, n_variables, "W_Model_OneMax", dummy_para, epistasis_para, neutrality_para, ruggedness_para)
        {
          objective_.x = this->epistasis_para == 0 ? std::vector<int>(n_variables,1) : std::vector<int>(0);
          objective_.y = {static_cast<double> (n_variables * (this->dummy_para == 0 ? 1 : this->dummy_para) / ( this->neutrality_para == 0 ? 1 : this->neutrality_para) )};
        }
      };
    }
  }
}
