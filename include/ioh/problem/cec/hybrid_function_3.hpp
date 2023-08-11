#pragma once

#include "cec_problem.hpp"
#include "functions.hpp"

namespace ioh::problem::cec
{
    class HybridFunction3 final : public CECProblem<HybridFunction3>
    {
    protected:

        double evaluate(const std::vector<double>& x) override
        {
            int i,tmp,cf_num=5;
            double fit[5];
            int G[5],G_nx[5];
            double Gp[5]={0.3,0.2,0.2,0.1,0.2};

            int nx = x.size();
            double* y = (double *)malloc(sizeof(double) * nx);

            tmp=0;
            for (i=0; i<cf_num-1; i++)
            {
                G_nx[i] = ceil(Gp[i]*nx);
                tmp += G_nx[i];
            }
            G_nx[cf_num-1]=nx-tmp;

            G[0]=0;
            for (i=1; i<cf_num; i++)
            {
                G[i] = G[i-1]+G_nx[i-1];
            }

            for (i=0; i<nx; i++)
            {
                y[i]=x[this->input_permutation_[i] - 1];
            }

            size_t total_size = 0;
            for (const auto &row : this->linear_transformation_) { total_size += row.size(); }
            std::vector<double> flat_data(total_size);
            size_t index = 0;
            for (const auto &row : this->linear_transformation_) { for (double val : row) { flat_data[index++] = val; } }
            double *linear_transformation_raw = &flat_data[0];

            i=0;
            katsuura_func(&y[G[i]],&fit[i],G_nx[i],&this->variables_shift_[0],linear_transformation_raw,0,0);
            i=1;
            happycat_func(&y[G[i]],&fit[i],G_nx[i],&this->variables_shift_[0],linear_transformation_raw,0,0);
            i=2;
            grie_rosen_func(&y[G[i]],&fit[i],G_nx[i],&this->variables_shift_[0],linear_transformation_raw,0,0);
            i=3;
            schwefel_func(&y[G[i]],&fit[i],G_nx[i],&this->variables_shift_[0],linear_transformation_raw,0,0);
            i=4;
            ackley_func(&y[G[i]],&fit[i],G_nx[i],&this->variables_shift_[0],linear_transformation_raw,0,0);
            double f = 0.0;
            for(i=0;i<cf_num;i++)
            {
                f += fit[i];
            }
            free(y);
            return f;
        }

        std::vector<double> transform_variables(std::vector<double> x) override
        {
            int nx = x.size();
            size_t total_size = 0;
            for (const auto &row : this->linear_transformation_) { total_size += row.size(); }
            std::vector<double> flat_data(total_size);
            size_t index = 0;
            for (const auto &row : this->linear_transformation_) { for (double val : row) { flat_data[index++] = val; } }
            double *linear_transformation_raw = &flat_data[0];
            double* z = (double *)malloc(sizeof(double) * nx);
            sr_func (&x[0], z, nx, &this->variables_shift_[0], linear_transformation_raw, 1.0, 1, 1);
            std::vector<double> z_vector(z, z + nx);
            free(z);
            return z_vector;
        }

    public:

        HybridFunction3(const int instance, const int n_variables) :
            CECProblem(8, instance, n_variables, "CEC_HybridFunction3")
        {
        }
    };
}
