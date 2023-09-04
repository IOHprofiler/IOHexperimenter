#pragma once

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
    #define INF 1.0e99
    #define EPS 1.0e-14
    #define E  2.7182818284590452353602874713526625
    #define PI 3.1415926535897932384626433832795029

    void shiftfunc (const double *x, double *xshift, int nx, double *Os)
    {
        for (int i = 0; i < nx; i++)
        {
            xshift[i] = x[i] - Os[i];
            LOG("shiftfunc, i: " << i << ", xshift[" << i << "]: " << xshift[i]);
        }
    }

    void rotatefunc (const double *x, double *xrot, int nx, double *Mr)
    {
        for (int i = 0; i < nx; i++)
        {
            xrot[i] = 0;
            for (int j = 0; j < nx; j++)
            {
                xrot[i] = xrot[i] + x[j] * Mr[i*nx + j];
            }
            LOG("rotatefunc, i: " << i << ", xrot[" << i << "]: " << xrot[i]);
        }
    }

    void sr_func (const double *x, double *sr_x, int nx, double *Os, double *Mr, double sh_rate, int s_flag, int r_flag)
    {
        double* y = (double *)malloc(sizeof(double) * nx);

        if (s_flag == 1)
        {
            if (r_flag == 1)
            {
                shiftfunc(x, y, nx, Os);
                for (int i = 0; i < nx; i++)
                {
                    y[i] = y[i] * sh_rate;
                    LOG("sr_func (s_flag==1 && r_flag==1), i: " << i << ", y[" << i << "]: " << y[i]);
                }
                rotatefunc(y, sr_x, nx, Mr);
            }
            else
            {
                shiftfunc(x, sr_x, nx, Os);
                for (int i = 0; i < nx; i++)
                {
                    sr_x[i] = sr_x[i] * sh_rate;
                    LOG("sr_func (s_flag==1 && r_flag!=1), i: " << i << ", sr_x[" << i << "]: " << sr_x[i]);
                }
            }
        }
        else
        {
            if (r_flag == 1)
            {
                for (int i = 0; i < nx; i++)
                {
                    y[i] = x[i] * sh_rate;
                    LOG("sr_func (s_flag!=1 && r_flag==1), i: " << i << ", y[" << i << "]: " << y[i]);
                }
                rotatefunc(y, sr_x, nx, Mr);
            }
            else
            {
                for (int i = 0; i < nx; i++)
                {
                    sr_x[i] = x[i] * sh_rate;
                    LOG("sr_func (s_flag!=1 && r_flag!=1), i: " << i << ", sr_x[" << i << "]: " << sr_x[i]);
                }
            }
        }

        free(y);
    }

    void bent_cigar_func (const double *x, double *f, int nx, double *Os,double *Mr, int s_flag, int r_flag) /* Bent_Cigar */
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i;
        sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag); /* shift and rotate */
        f[0] = z[0] * z[0];
        LOG("bent_cigar_func f[0] (using z[0]): " << f[0]);  // Print initial f[0] value
        LOG("Initial f[0] (using z[0]): " << f[0]);  // Print initial f[0] value
        for (i=1; i<nx; i++)
        {
            LOG("Iteration: " << i);        // Print current iteration number
            LOG("z[" << i << "]: " << z[i]); // Print current z value
            LOG("x[" << i << "]: " << x[i]); // Print current x value

            f[0] += pow(10.0, 6.0) * z[i] * z[i];

            LOG("f[0] after iteration " << i << ": " << f[0]);  // Print current f[0] value
        }
        free(z);
    }

    void hgbat_func (const double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag)
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

    void rastrigin_func (const double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag)
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

    void katsuura_func (const double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag)
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

    void happycat_func (const double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag)
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

    void grie_rosen_func (const double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag)
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

    void schwefel_func (const double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* Schwefel's  */
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

    void ackley_func (const double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag)
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
    void schaffer_F7_func (const double *x, double *f, double *y, int nx, double *Os,double *Mr,int s_flag, int r_flag)
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

    void cf_cal(const double *x, double *f, int nx, double *Os,double * delta,double * bias,double * fit, int cf_num)
    {
        int i,j;
        double *w;
        double w_max=0,w_sum=0;
        w=(double *)malloc(cf_num * sizeof(double));
        for (i=0; i<cf_num; i++)
        {
            fit[i]+=bias[i];
            w[i]=0;
            for (j=0; j<nx; j++)
            {
                w[i]+=pow(x[j]-Os[i*nx+j],2.0);
            }
            if (w[i]!=0)
                w[i]=pow(1.0/w[i],0.5)*exp(-w[i]/2.0/nx/pow(delta[i],2.0));
            else
                w[i]=INF;
            if (w[i]>w_max)
                w_max=w[i];
        }

        for (i=0; i<cf_num; i++)
        {
            w_sum=w_sum+w[i];
        }
        if(w_max==0)
        {
            for (i=0; i<cf_num; i++)
                w[i]=1;
            w_sum=cf_num;
        }
        f[0] = 0.0;
        for (i=0; i<cf_num; i++)
        {
            LOG("Iteration: " << i);         // Print current iteration number
            LOG("w[" << i << "]: " << w[i]); // Print current w value
            LOG("fit[" << i << "]: " << fit[i]); // Print current fit value

            f[0] = f[0] + w[i] / w_sum * fit[i];

            LOG("f[0] after iteration " << i << ": " << f[0]); // Print current f[0] value
        }
        free(w);
    }

    void rosenbrock_func (const double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* Rosenbrock's */
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i;
        double tmp1,tmp2;
        f[0] = 0.0;
        sr_func (x, z, nx, Os, Mr, 2.048/100.0, s_flag, r_flag); /* shift and rotate */
        z[0] += 1.0;//shift to orgin
        for (i=0; i<nx-1; i++)
        {
            z[i+1] += 1.0;//shift to orgin
            tmp1=z[i]*z[i]-z[i+1];
            tmp2=z[i]-1.0;
            f[0] += 100.0*tmp1*tmp1 +tmp2*tmp2;
        }
        free(z);
    }

    void ellips_func (const double *x, double *f, int nx, double *Os,double *Mr, int s_flag, int r_flag) /* Ellipsoidal */
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i;
        f[0] = 0.0;
        sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag); /* shift and rotate */
        for (i=0; i<nx; i++)
        {
           f[0] += pow(10.0,6.0*i/(nx-1))*z[i]*z[i];
        }
        free(z);
    }

    void discus_func (const double *x, double *f, int nx, double *Os,double *Mr, int s_flag, int r_flag) /* Discus */
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i;
        sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag); /* shift and rotate */
        f[0] = pow(10.0,6.0)*z[0]*z[0];
        for (i=1; i<nx; i++)
        {
            f[0] += z[i]*z[i];
        }
        free(z);
    }

    void escaffer6_func (const double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag)
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i;
        double temp1, temp2;

        sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag); /* shift and rotate */

        f[0] = 0.0;
        for (i=0; i<nx-1; i++)
        {
            LOG("escaffer6_func loop, i: " << i << ", z[" << i << "]: " << z[i]);  // LOGGING

            temp1 = sin(sqrt(z[i]*z[i]+z[i+1]*z[i+1]));
            temp1 =temp1*temp1;
            temp2 = 1.0 + 0.001*(z[i]*z[i]+z[i+1]*z[i+1]);
            f[0] += 0.5 + (temp1-0.5)/(temp2*temp2);
        }
        temp1 = sin(sqrt(z[nx-1]*z[nx-1]+z[0]*z[0]));
        temp1 =temp1*temp1;
        temp2 = 1.0 + 0.001*(z[nx-1]*z[nx-1]+z[0]*z[0]);
        f[0] += 0.5 + (temp1-0.5)/(temp2*temp2);
        free(z);
    }

    void griewank_func (const double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag)
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        int i;
        double s, p;
        s = 0.0;
        p = 1.0;

        sr_func (x, z, nx, Os, Mr, 600.0/100.0, s_flag, r_flag); /* shift and rotate */

        for (i=0; i<nx; i++)
        {
            LOG("griewank_func loop, i: " << i << ", z[" << i << "]: " << z[i]);  // LOGGING

            s += z[i]*z[i];
            p *= cos(z[i]/sqrt(1.0+i));
        }
        f[0] = 1.0 + s/4000.0 - p;
        free(z);
    }

    void hf10 (const double *x, double *f, int nx, double *Os,double *Mr,int *S,int s_flag,int r_flag)
    {
        double* z=(double *)malloc(sizeof(double)  *  nx);
        double* y=(double *)malloc(sizeof(double)  *  nx);
        int i,tmp,cf_num=6;
        double fit[6];
        int G[6],G_nx[6];
        double Gp[6]={0.1,0.2,0.2,0.2,0.1,0.2};

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

        sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag); /* shift and rotate */

        for (i=0; i<nx; i++)
        {
            y[i]=z[S[i]-1];
        }

        i=0;
        hgbat_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0);
        i=1;
        katsuura_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0);
        i=2;
        ackley_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0);
        i=3;
        rastrigin_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0);
        i=4;
        schwefel_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0);
        i=5;
        schaffer_F7_func(&y[G[i]],&fit[i],y,G_nx[i],Os,Mr,0,0);

        f[0]=0.0;
        for(i=0;i<cf_num;i++)
        {
            f[0] += fit[i];
        }
        free(z);
        free(y);
    }

    void cf01 (const double *x, double *f, int nx, double *Os,double *Mr,int r_flag) /* Composition Function 1 */
    {
        int i,cf_num=5;
        double fit[5];
        double delta[5] = {10, 20, 30, 40, 50};
        double bias[5] = {0, 200, 300, 100, 400};

        i=0;
        rosenbrock_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        fit[i]=10000*fit[i]/1e+4;
        i=1;
        ellips_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        fit[i]=10000*fit[i]/1e+10;
        i=2;
        bent_cigar_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        fit[i]=10000*fit[i]/1e+30;
        i=3;
        discus_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        fit[i]=10000*fit[i]/1e+10;
        i=4;
        ellips_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,0);
        fit[i]=10000*fit[i]/1e+10;
        cf_cal(x, f, nx, Os, delta,bias,fit,cf_num);
    }

    void cf02 (const double *x, double *f, int nx, double *Os,double *Mr,int r_flag) /* Composition Function 2 */
    {
        int i,cf_num=3;
        double fit[3];
        double delta[3] = {20,10,10};
        double bias[3] = {0, 200, 100};

        i=0;
        schwefel_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,0);
        i=1;
        rastrigin_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        i=2;
        hgbat_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        cf_cal(x, f, nx, Os, delta,bias,fit,cf_num);
    }

    void cf06 (const double *x, double *f, int nx, double *Os,double *Mr,int r_flag) /* Composition Function 4 */
    {
        int i,cf_num=5;
        double fit[5];
        double delta[5] = {20,20,30,30,20};
        double bias[5] = {0, 200, 300, 400, 200};
        i=0;
        escaffer6_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        fit[i]=10000*fit[i]/2e+7;
        i=1;
        schwefel_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        i=2;
        griewank_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        fit[i]=1000*fit[i]/100;
        i=3;
        rosenbrock_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        i=4;
        rastrigin_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        fit[i]=10000*fit[i]/1e+3;
        cf_cal(x, f, nx, Os, delta,bias,fit,cf_num);
    }

    void cf07 (const double *x, double *f, int nx, double *Os,double *Mr,int r_flag) /* Composition Function 4 */
    {
        int i,cf_num=6;
        double fit[6];
        double delta[6] = {10,20,30,40,50,60};
        double bias[6] = {0, 300, 500, 100, 400, 200};
        i=0;
        hgbat_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        fit[i]=10000*fit[i]/1000;
        i=1;
        rastrigin_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        fit[i]=10000*fit[i]/1e+3;
        i=2;
        schwefel_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        fit[i]=10000*fit[i]/4e+3;
        i=3;
        bent_cigar_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        fit[i]=10000*fit[i]/1e+30;
        i=4;
        ellips_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        fit[i]=10000*fit[i]/1e+10;
        i=5;
        escaffer6_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
        fit[i]=10000*fit[i]/2e+7;
        cf_cal(x, f, nx, Os, delta,bias,fit,cf_num);
    }
} // namespace ioh::problem::cec
