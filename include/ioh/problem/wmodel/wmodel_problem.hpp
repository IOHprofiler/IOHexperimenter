#pragma once

#include "ioh/problem/problem.hpp"
#include "ioh/problem/utils.hpp"

namespace ioh::problem
{
    class WModel : public Integer
    {
    protected:
        std::vector<int> dummy_info;
        double dummy_para = 0;
        int epistasis_para = 0;
        int neutrality_para = 0;
        int ruggedness_para = 0;
        std::vector<int> ruggedness_info;
      
        std::vector<int> transform_variables(std::vector<int> x) override
        {
            if (meta_data_.instance > 1 && meta_data_.instance <= 50)
                transformation::methods::transform_vars_xor(x, meta_data_.instance);
            else if (meta_data_.instance > 50 && meta_data_.instance <= 100)
                transformation::methods::transform_vars_sigma(x, meta_data_.instance);
            return x;
        }

        std::vector<double> transform_objectives(std::vector<double> y) override
        {
            if (meta_data_.instance > 1)
            {
                transformation::methods::transform_obj_scale(y, meta_data_.instance);
                transformation::methods::transform_obj_shift(y, meta_data_.instance);
            }
            return y;
        }
      
        void prepare_problem()
        {
          auto temp_dimension =  this->meta_data_.n_variables;
        
          if (this->dummy_para > 0)
          {
            this->dummy_info = utils::dummy(temp_dimension, dummy_para, 10000);
            assert(this->dummy_info.size() == static_cast<size_t>(temp_dimension * this->dummy_para));
            temp_dimension = this->dummy_info.size();
          }
          
          if (this->neutrality_para > 0)
          {
            temp_dimension = temp_dimension / this->neutrality_para;
          }
          if (this->ruggedness_para > 0)
          {
            this->ruggedness_info = utils::ruggedness_raw(utils::ruggedness_translate(this->ruggedness_para,temp_dimension),temp_dimension);
          }
      }

    public:
        WModel(const int problem_id, const int instance, const int n_variables, const std::string &name, const double dummy_para,const int epistasis_para, const int neutrality_para, const double ruggedness_para) :
            Integer(MetaData(problem_id, instance, name, n_variables, 1,
                                    common::OptimizationType::Maximization))
        {
          this->dummy_para = 1 - dummy_para;
          this->epistasis_para = epistasis_para;
          this->neutrality_para = neutrality_para;
          this->ruggedness_para = ruggedness_para * n_variables;
          this->prepare_problem();
        }
    };

    template <typename ProblemType>
    class WModelProblem : public WModel,
               AutomaticProblemRegistration<ProblemType, WModel>,
               AutomaticProblemRegistration<ProblemType, Integer>
    {
    public:
        using WModel::WModel;
    };
}