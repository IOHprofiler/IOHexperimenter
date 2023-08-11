#pragma once

namespace ioh::problem::cec
{
    #define E  2.7182818284590452353602874713526625
    #define PI 3.1415926535897932384626433832795029

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
        double* y = (double *)malloc(sizeof(double) * nx);
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
            f[0] += (z[i]*z[i] - 10.0*cos(2.0*PI*z[i]) + 10.0);
        }
        free(z);
    }

    void katsuura_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag)
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i,j;
        double temp,tmp1,tmp2,tmp3;
        f[0]=1.0;
        tmp3=pow(1.0*nx,1.2);

        sr_func (x, z, nx, Os, Mr, 5.0/100.0, s_flag, r_flag); /* shift and rotate */

        for (i=0; i<nx; i++)
        {
            temp=0.0;
            for (j=1; j<=32; j++)
            {
                tmp1=pow(2.0,j);
                tmp2=tmp1*z[i];
                temp += fabs(tmp2-floor(tmp2+0.5))/tmp1;
            }
            f[0] *= pow(1.0+(i+1)*temp,10.0/tmp3);
        }
        tmp1=10.0/nx/nx;
        f[0]=f[0]*tmp1-tmp1;
        free(z);
    }

    void happycat_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag)
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i;
        double alpha,r2,sum_z;
        alpha=1.0/8.0;

        sr_func (x, z, nx, Os, Mr, 5.0/100.0, s_flag, r_flag); /* shift and rotate */

        r2 = 0.0;
        sum_z=0.0;
        for (i=0; i<nx; i++)
        {
            z[i]=z[i]-1.0;//shift to orgin
            r2 += z[i]*z[i];
            sum_z += z[i];
        }
        f[0]=pow(fabs(r2-nx),2*alpha) + (0.5*r2 + sum_z)/nx + 0.5;
        free(z);
    }

    void grie_rosen_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag)
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i;
        double temp,tmp1,tmp2;
        f[0]=0.0;

        sr_func (x, z, nx, Os, Mr, 5.0/100.0, s_flag, r_flag); /* shift and rotate */

        z[0] += 1.0;//shift to orgin
        for (i=0; i<nx-1; i++)
        {
            z[i+1] += 1.0;//shift to orgin
            tmp1 = z[i]*z[i]-z[i+1];
            tmp2 = z[i]-1.0;
            temp = 100.0*tmp1*tmp1 + tmp2*tmp2;
             f[0] += (temp*temp)/4000.0 - cos(temp) + 1.0;
        }
        tmp1 = z[nx-1]*z[nx-1]-z[0];
        tmp2 = z[nx-1]-1.0;
        temp = 100.0*tmp1*tmp1 + tmp2*tmp2;;
        f[0] += (temp*temp)/4000.0 - cos(temp) + 1.0 ;
        free(z);
    }

    void schwefel_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* Schwefel's  */
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i;
        double tmp;
        f[0]=0.0;

        sr_func (x, z, nx, Os, Mr, 1000.0/100.0, s_flag, r_flag); /* shift and rotate */

        for (i=0; i<nx; i++)
        {
            z[i] += 4.209687462275036e+002;
            if (z[i]>500)
            {
                f[0]-=(500.0-fmod(z[i],500))*sin(pow(500.0-fmod(z[i],500),0.5));
                tmp=(z[i]-500.0)/100;
                f[0]+= tmp*tmp/nx;
            }
            else if (z[i]<-500)
            {
                f[0]-=(-500.0+fmod(fabs(z[i]),500))*sin(pow(500.0-fmod(fabs(z[i]),500),0.5));
                tmp=(z[i]+500.0)/100;
                f[0]+= tmp*tmp/nx;
            }
            else
                f[0]-=z[i]*sin(pow(fabs(z[i]),0.5));
        }
        f[0] +=4.189828872724338e+002*nx;
        free(z);
    }

    void ackley_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag)
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i;
        double sum1, sum2;
        sum1 = 0.0;
        sum2 = 0.0;

        sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag); /* shift and rotate */

        for (i=0; i<nx; i++)
        {
            sum1 += z[i]*z[i];
            sum2 += cos(2.0*PI*z[i]);
        }
        sum1 = -0.2*sqrt(sum1/nx);
        sum2 /= nx;
        f[0] =  E - 20.0*exp(sum1) - exp(sum2) +20.0;
        free(z);
    }

    // y is the x, permuted by S; this y value is used in computing Hybrid Function 2
    void schaffer_F7_func (double *x, double *f, double *y, int nx, double *Os,double *Mr,int s_flag, int r_flag)
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i;
        double tmp;
        f[0] = 0.0;
        sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag); /* shift and rotate */
        for (i=0; i<nx-1; i++)
        {
        z[i]=pow(y[i]*y[i]+y[i+1]*y[i+1],0.5);
        tmp=sin(50.0*pow(z[i],0.2));
        f[0] += pow(z[i],0.5)+pow(z[i],0.5)*tmp*tmp ;
        }
        f[0] = f[0]*f[0]/(nx-1)/(nx-1);
        free(z);
    }
} // namespace ioh::problem::cec
