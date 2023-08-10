#include "ioh/common/random.hpp" // defines IOH_PI
#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    void shiftfunc (const double *x, double *xshift, int nx,double *Os)
    {
        int i;
        for (i=0; i<nx; i++)
        {
            xshift[i]=x[i]-Os[i];
        }
    }

    void rotatefunc (const double *x, double *xrot, int nx,double *Mr)
    {
        int i,j;
        for (i=0; i<nx; i++)
        {
            xrot[i]=0;
                for (j=0; j<nx; j++)
                {
                    xrot[i]=xrot[i]+x[j]*Mr[i*nx+j];
                }
        }
    }

    void sr_func (const double *x, double *sr_x, int nx, double *Os,double *Mr, double sh_rate, int s_flag,int r_flag) /* shift and rotate */
    {
        double* y=(double *)malloc(sizeof(double)  *  nx);
        int i;
        if (s_flag==1)
        {
            if (r_flag==1)
            {
                shiftfunc(x, y, nx, Os);
                for (i=0; i<nx; i++)//shrink to the orginal search range
                {
                    y[i]=y[i]*sh_rate;
                }
                rotatefunc(y, sr_x, nx, Mr);
            }
            else
            {
                shiftfunc(x, sr_x, nx, Os);
                for (i=0; i<nx; i++)//shrink to the orginal search range
                {
                    sr_x[i]=sr_x[i]*sh_rate;
                }
            }
        }
        else
        {

            if (r_flag==1)
            {
                for (i=0; i<nx; i++)//shrink to the orginal search range
                {
                    y[i]=x[i]*sh_rate;
                }
                rotatefunc(y, sr_x, nx, Mr);
            }
            else
            for (i=0; i<nx; i++)//shrink to the orginal search range
            {
                sr_x[i]=x[i]*sh_rate;
            }
        }
        free(y);
    }

    void bent_cigar_func (const double *x, double *f, int nx, double *Os,double *Mr, int s_flag, int r_flag) /* Bent_Cigar */
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i;
        sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag); /* shift and rotate */
        f[0] = z[0]*z[0];
        for (i=1; i<nx; i++)
        {
            f[0] += pow(10.0,6.0)*z[i]*z[i];
        }
        free(z);
    }

    void hgbat_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag)
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i;
        double alpha,r2,sum_z;
        alpha=1.0/4.0;

        sr_func (x, z, nx, Os, Mr, 5.0/100.0, s_flag, r_flag); /* shift and rotate */

        r2 = 0.0;
        sum_z=0.0;
        for (i=0; i<nx; i++)
        {
            z[i]=z[i]-1.0;//shift to orgin
            r2 += z[i]*z[i];
            sum_z += z[i];
        }
        // Calculate inner powers
        double&& r2_squared = pow(r2, 2.0);
        double&& sum_z_squared = pow(sum_z, 2.0);

        // Compute the difference of the squares
        double&& diff_squares = r2_squared - sum_z_squared;

        // Apply the outer power with alpha
        double&& powered_diff = pow(fabs(diff_squares), 2 * alpha);

        // Compute the additional terms
        double&& term2 = 0.5 * r2;
        double&& term3 = sum_z / nx;

        // Final assembly of the components into f[0]
        f[0] = powered_diff + term2 + term3 + 0.5;
        free(z);
    }

    void rastrigin_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag)
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i;
        f[0] = 0.0;

        sr_func (x, z, nx, Os, Mr, 5.12/100.0, s_flag, r_flag); /* shift and rotate */

        for (i=0; i<nx; i++)
        {
            f[0] += (z[i]*z[i] - 10.0*cos(2.0*IOH_PI*z[i]) + 10.0);
        }
        free(z);
    }
} // namespace ioh::problem::cec
