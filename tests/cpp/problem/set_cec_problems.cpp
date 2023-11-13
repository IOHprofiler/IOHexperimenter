/*
  CEC22 Test Function Suite for Single Objective Bound Constrained Numerical Optimization


*/

#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <malloc.h>
#include <math.h>
#include <numeric>
#include <stdio.h>
#include <vector>

// ===============================================================================================================================
#define DEBUG
#ifdef DEBUG
#define LOG_FILE_NAME "cec_training_log.txt"

#define LOG(message)                                                           \
  do {                                                                         \
    std::ofstream debug_log(LOG_FILE_NAME, std::ios::app);                     \
    auto now = std::chrono::system_clock::now();                               \
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);          \
    debug_log << "["                                                           \
              << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") \
              << "] " << message << std::endl;                                 \
    debug_log.close();                                                         \
  } while (0)

#else
#define LOG(message) // Nothing
#endif

// Output stream operator for std::vector<double>
std::ostream &operator<<(std::ostream &os, const std::vector<double> &vec) {
  os << "[";
  for (size_t i = 0; i < vec.size(); ++i) {
    os << vec[i];
    if (i != vec.size() - 1) {
      os << ", ";
    }
  }
  os << "]";
  return os;
}
// ===============================================================================================================================

#define INF 1.0e99
#define EPS 1.0e-14
#define E  2.7182818284590452353602874713526625
#define PI 3.1415926535897932384626433832795029


void ackley_func (double *, double *, int , double *,double *, int, int); /* Ackley's */
void bent_cigar_func(double *, double *, int , double *,double *, int, int); /* Discus */
void discus_func(double *, double *, int , double *,double *, int, int);  /* Bent_Cigar */
void ellips_func(double *, double *, int , double *,double *, int, int); /* Ellipsoidal */
void escaffer6_func (double *, double *, int , double *,double *, int, int); /* Expanded Scaffer¡¯s F6  */
void griewank_func (double *, double *, int , double *,double *, int, int); /* Griewank's  */
void grie_rosen_func (double *, double *, int , double *,double *, int, int); /* Griewank-Rosenbrock  */
void happycat_func (double *, double *, int , double *,double *, int, int); /* HappyCat */
void hgbat_func (double *, double *, int , double *,double *, int, int); /* HGBat  */
void rosenbrock_func (double *, double *, int , double *,double *, int, int); /* Rosenbrock's */
void rastrigin_func (double *, double *, int , double *,double *, int, int); /* Rastrigin's  */
void schwefel_func (double *, double *, int , double *,double *, int, int); /* Schwefel's */
void schaffer_F7_func (double *, double *, int , double *,double *, int, int); /* Schwefel's F7 */
void step_rastrigin_func (double *, double *, int , double *,double *, int, int); /* Noncontinuous Rastrigin's  */
void levy_func(double *, double *, int , double *,double *, int, int); /* Levy */
void zakharov_func(double *, double *, int , double *,double *, int, int); /* ZAKHAROV */
void katsuura_func (double *, double *, int , double *,double *, int, int); /* Katsuura */



void hf02 (double *, double *, int, double *,double *, int *,int, int); /* Hybrid Function 2 */
void hf06 (double *, double *, int, double *,double *, int *,int, int); /* Hybrid Function 6 */
void hf10 (double *, double *, int, double *,double *, int *,int, int); /* Hybrid Function 10 */


void cf01 (double *, double *, int , double *,double *, int); /* Composition Function 1 */
void cf02 (double *, double *, int , double *,double *, int); /* Composition Function 2 */
void cf06 (double *, double *, int , double *,double *, int); /* Composition Function 6 */
void cf07 (double *, double *, int , double *,double *, int); /* Composition Function 7 */



void shiftfunc (double*,double*,int,double*);
void rotatefunc (double*,double*,int, double*);
void sr_func (double *, double *, int, double*, double*, double, int, int); /* shift and rotate */
void asyfunc (double *, double *x, int, double);
void oszfunc (double *, double *, int);
void cf_cal(double *, double *, int, double *,double *,double *,double *,int);

double *OShift,*M,*y,*z,*x_bound;
int ini_flag,n_flag,func_flag,*SS;




void cec22_test_func(double *x, double *f, int nx, int mx,int func_num)
{

    int cf_num=12,i;
    if (func_num<1||func_num>12)
    {
      printf("\nError: Test function %d is not defined.\n", func_num);
    }
  if (ini_flag==1)
  {
    if ((n_flag!=nx)||(func_flag!=func_num))
    {
      ini_flag=0;
    }
  }

  if (ini_flag==0)
  {
    FILE *fpt;
    char FileName[256];
    free(M);
    free(OShift);
    free(y);
    free(z);
    free(x_bound);
    y = static_cast<double*>(malloc(nx * sizeof(double)));
    z = static_cast<double*>(malloc(nx * sizeof(double)));
    x_bound = static_cast<double*>(malloc(nx * sizeof(double)));
    for (i=0; i<nx; i++)
      x_bound[i]=100.0;

    if (!(nx==2||nx==10||nx==20))
    {
      printf("\nError: Test functions are only defined for D=2,10,20.\n");
    }
    if (nx==2&&(func_num==6||func_num==7||func_num==8))
    {
      printf("\nError:  NOT defined for D=2.\n");
    }

    /* Load Matrix M*/
    sprintf(FileName, "input_data/M_%d_D%d.txt", func_num,nx);
    fpt = fopen(FileName,"r");
    if (fpt==NULL)
    {
        printf("\n Error: Cannot open M_%d_D%d.txt for reading \n",func_num,nx);
    }
    if (func_num<9)
    {
      M = static_cast<double*>(malloc(nx * nx * sizeof(double)));
      if (M==NULL)
        printf("\nError: there is insufficient memory available!\n");
      for (i=0; i<nx*nx; i++)
      {
        fscanf(fpt,"%lf",&M[i]);
      }
    }
    else
    {
      M = static_cast<double*>(malloc(cf_num * nx * nx * sizeof(double)));
      if (M==NULL)
        printf("\nError: there is insufficient memory available!\n");
      for (i=0; i<cf_num*nx*nx; i++)
      {
        fscanf(fpt,"%lf",&M[i]);
      }
    }
    fclose(fpt);

    /* Load shift_data */
    sprintf(FileName, "input_data/shift_data_%d.txt", func_num);
    fpt = fopen(FileName,"r");
    if (fpt==NULL)
    {
      printf("\n Error: Cannot open shift_data_%d.txt for reading \n",func_num);
    }

    if (func_num<9)
    {
      OShift = static_cast<double*>(malloc(nx * sizeof(double)));
      if (OShift==NULL)
      printf("\nError: there is insufficient memory available!\n");
      for(i=0;i<nx;i++)
      {
        fscanf(fpt,"%lf",&OShift[i]);
      }
    }
    else
    {
            //OShift=(double *)malloc(nx*sizeof(double));
      OShift = static_cast<double*>(malloc(nx * cf_num * sizeof(double)));
      if (OShift==NULL)
      printf("\nError: there is insufficient memory available!\n");
      for(i=0;i<cf_num-1;i++)
      {
        for (int j=0;j<nx;j++)
        {
          fscanf(fpt,"%lf",&OShift[i*nx+j]);

        }
        fscanf(fpt,"%*[^\n]%*c");
      }
      for (int j=0;j<nx;j++)
      {
        fscanf(fpt,"%lf",&OShift[nx*(cf_num-1)+j]);

      }

    }
    fclose(fpt);


    /* Load Shuffle_data */

    if (func_num>=6&&func_num<=8)
    {
      sprintf(FileName, "input_data/shuffle_data_%d_D%d.txt", func_num, nx);
      fpt = fopen(FileName,"r");
      if (fpt==NULL)
      {
        printf("\n Error: Cannot open shuffle_data_%d_D%d.txt for reading \n", func_num, nx);
      }
      SS = static_cast<int*>(malloc(nx * sizeof(int)));
      if (SS==NULL)
        printf("\nError: there is insufficient memory available!\n");
      for(i=0;i<nx;i++)
      {
        fscanf(fpt,"%d",&SS[i]);
      }
      fclose(fpt);
    }

    n_flag=nx;
    func_flag=func_num;
    ini_flag=1;
    //printf("Function has been initialized!\n");
  }


  for (i = 0; i < mx; i++)
  {
    switch(func_num)
    {
    case 1:
      zakharov_func(&x[i*nx],&f[i],nx,OShift,M,1,1);
      f[i]+=300.0;
      break;
    case 2:
      rosenbrock_func(&x[i*nx],&f[i],nx,OShift,M,1,1);
      f[i]+=400.0;
      break;
    case 3:
      schaffer_F7_func(&x[i*nx],&f[i],nx,OShift,M,1,1);
      f[i]+=600.0;
      break;
    case 4:
            step_rastrigin_func(&x[i*nx],&f[i],nx,OShift,M,1,1);
      f[i]+=800.0;
      break;
    case 5:
      levy_func(&x[i*nx],&f[i],nx,OShift,M,1,1);
      f[i]+=900.0;
      break;
    case 6:
      hf02(&x[i*nx],&f[i],nx,OShift,M,SS,1,1);
      f[i]+=1800.0;
      break;
    case 7:
      hf10(&x[i*nx],&f[i],nx,OShift,M,SS,1,1);
      f[i]+=2000.0;
      break;
    case 8:
      hf06(&x[i*nx],&f[i],nx,OShift,M,SS,1,1);
      f[i]+=2200.0;
      break;
    case 9:
      cf01(&x[i*nx],&f[i],nx,OShift,M,1);
      f[i]+=2300.0;
      break;
    case 10:
      cf02(&x[i*nx],&f[i],nx,OShift,M,1);
      f[i]+=2400.0;
      break;
        case 11:
            cf06(&x[i*nx],&f[i],nx,OShift,M,1);
      f[i]+=2600.0;
      break;
        case 12:
            cf07(&x[i*nx],&f[i],nx,OShift,M,1);
      f[i]+=2700.0;
      break;
    default:
      printf("\nError: There are only 10 test functions in this test suite!\n");
      f[i] = 0.0;
      break;
    }

  }

}



void ellips_func (double *x, double *f, int nx, double *Os,double *Mr, int s_flag, int r_flag) /* Ellipsoidal */
{
    int i;
  f[0] = 0.0;
  sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag); /* shift and rotate */
  for (i=0; i<nx; i++)
  {
       f[0] += pow(10.0,6.0*i/(nx-1))*z[i]*z[i];
  }
}



void bent_cigar_func (double *x, double *f, int nx, double *Os,double *Mr, int s_flag, int r_flag) /* Bent_Cigar */
{
    int i;
  sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag); /* shift and rotate */
  f[0] = z[0]*z[0];
  for (i=1; i<nx; i++)
  {
    f[0] += pow(10.0,6.0)*z[i]*z[i];
  }

}

void discus_func (double *x, double *f, int nx, double *Os,double *Mr, int s_flag, int r_flag) /* Discus */
{
    int i;
  sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag); /* shift and rotate */
  f[0] = pow(10.0,6.0)*z[0]*z[0];
  for (i=1; i<nx; i++)
  {
    f[0] += z[i]*z[i];
  }
}



void rosenbrock_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* Rosenbrock's */
{
    int i;
  f[0] = 0.0;
  sr_func (x, z, nx, Os, Mr, 2.048/100.0, s_flag, r_flag); /* shift and rotate */
  z[0] += 1.0;//shift to orgin
  for (i=0; i<nx-1; i++)
  {
    z[i+1] += 1.0;//shift to orgin
    double tmp1=z[i]*z[i]-z[i+1];
    double tmp2=z[i]-1.0;
    f[0] += 100.0*tmp1*tmp1 +tmp2*tmp2;
  }
}



void ackley_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* Ackley's  */
{
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
}




void griewank_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* Griewank's  */
{
    int i;
    double s, p;
    s = 0.0;
    p = 1.0;

  sr_func (x, z, nx, Os, Mr, 600.0/100.0, s_flag, r_flag); /* shift and rotate */

  for (i=0; i<nx; i++)
  {
    s += z[i]*z[i];
    p *= cos(z[i]/sqrt(1.0+i));
  }
  f[0] = 1.0 + s/4000.0 - p;
}

void rastrigin_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* Rastrigin's  */
{
    int i;
  f[0] = 0.0;

  sr_func (x, z, nx, Os, Mr, 5.12/100.0, s_flag, r_flag); /* shift and rotate */

  for (i=0; i<nx; i++)
  {
    f[0] += (z[i]*z[i] - 10.0*cos(2.0*PI*z[i]) + 10.0);
  }
}



void schwefel_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* Schwefel's  */
{
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

}



void grie_rosen_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* Griewank-Rosenbrock  */
{
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
}


void escaffer6_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* Expanded Scaffer��s F6  */
{
    int i;
    double temp1, temp2;

  sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag); /* shift and rotate */

    f[0] = 0.0;
    for (i=0; i<nx-1; i++)
    {
        temp1 = sin(sqrt(z[i]*z[i]+z[i+1]*z[i+1]));
    temp1 =temp1*temp1;
        temp2 = 1.0 + 0.001*(z[i]*z[i]+z[i+1]*z[i+1]);
        f[0] += 0.5 + (temp1-0.5)/(temp2*temp2);
    }
    temp1 = sin(sqrt(z[nx-1]*z[nx-1]+z[0]*z[0]));
  temp1 =temp1*temp1;
    temp2 = 1.0 + 0.001*(z[nx-1]*z[nx-1]+z[0]*z[0]);
    f[0] += 0.5 + (temp1-0.5)/(temp2*temp2);
}

void happycat_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* HappyCat, provdided by Hans-Georg Beyer (HGB) */
/* original global optimum: [-1,-1,...,-1] */
{
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
}

void hgbat_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* HGBat, provdided by Hans-Georg Beyer (HGB)*/
/* original global optimum: [-1,-1,...,-1] */
{
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
    f[0]=pow(fabs(pow(r2,2.0)-pow(sum_z,2.0)),2*alpha) + (0.5*r2 + sum_z)/nx + 0.5;
}


void schaffer_F7_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* Schwefel's 1.2  */
{
    int i;
    f[0] = 0.0;
  sr_func (x, z, nx, Os, Mr, 0.5/100, s_flag, r_flag); /* shift and rotate */
  for (i=0; i<nx-1; i++)
  {
    z[i]=pow(y[i]*y[i]+y[i+1]*y[i+1],0.5);
    double tmp=sin(50.0*pow(z[i],0.2));
    f[0] += pow(z[i],0.5)+pow(z[i],0.5)*tmp*tmp ;
  }
  f[0] = f[0]*f[0]/(nx-1)/(nx-1);
}

void step_rastrigin_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* Noncontinuous Rastrigin's  */
{
    int i;
  f[0]=0.0;
  for (i=0; i<nx; i++)
  {
    if (fabs(y[i]-Os[i])>0.5)
    y[i]=Os[i]+floor(2*(y[i]-Os[i])+0.5)/2;
  }

  sr_func (x, z, nx, Os, Mr, 5.12/100.0, s_flag, r_flag); /* shift and rotate */

  for (i=0; i<nx; i++)
  {
    f[0] += (z[i]*z[i] - 10.0*cos(2.0*PI*z[i]) + 10.0);
  }
}

void levy_func (double *x, double *f, int nx, double *Os,double *Mr, int s_flag, int r_flag) /* Levy */
{
    int i;
  f[0] = 0.0;
  sr_func (x, z, nx, Os, Mr,5.12/100, s_flag, r_flag); /* shift and rotate */

  double *w;
  w = static_cast<double*>(malloc(sizeof(double) * nx));

  for (i=0; i<nx; i++)
  {
     w[i] = 1.0 + (z[i] - 0.0)/4.0;
  }

  double term1 = pow((sin(PI*w[0])),2);
  double term3 = pow((w[nx-1]-1),2) * (1+pow((sin(2*PI*w[nx-1])),2));

  double sum = 0.0;

  for (i=0; i<nx-1; i++)
  {
    double wi = w[i];
        double newv = pow((wi-1),2) * (1+10*pow((sin(PI*wi+1)),2));
    sum = sum + newv;
  }

  f[0] = term1 + sum + term3;// - 1.442600987052770; // - 1.442600987052770
  free(w);   // ADD THIS LINE to free memory! Thanks for Dr. Janez
}

void zakharov_func (double *x, double *f, int nx, double *Os,double *Mr, int s_flag, int r_flag) /* zakharov */
{
  int i;
  sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag); // shift and rotate
  f[0] = 0.0;
  double sum1 = 0.0;
  double sum2 = 0.0;
  for (i=0; i<nx; i++)
  {
    double xi = z[i];
    sum1 = sum1 + pow(xi,2);
    sum2 = sum2 + 0.5*(i+1)*xi;
  }

  f[0] = sum1 + pow(sum2,2) + pow(sum2,4);
}

void katsuura_func (double *x, double *f, int nx, double *Os,double *Mr,int s_flag, int r_flag) /* Katsuura  */
{
    int i,j;
  double tmp1;
  f[0]=1.0;
  double tmp3=pow(1.0*nx,1.2);

  sr_func (x, z, nx, Os, Mr, 5.0/100.0, s_flag, r_flag); /* shift and rotate */

    for (i=0; i<nx; i++)
  {
    double temp=0.0;
    for (j=1; j<=32; j++)
    {
      tmp1=pow(2.0,j);
      double tmp2=tmp1*z[i];
      temp += fabs(tmp2-floor(tmp2+0.5))/tmp1;
    }
    f[0] *= pow(1.0+(i+1)*temp,10.0/tmp3);
    }
  tmp1=10.0/nx/nx;
    f[0]=f[0]*tmp1-tmp1;

}


void hf02 (double *x, double *f, int nx, double *Os,double *Mr,int *S,int s_flag,int r_flag) /* Hybrid Function 2 */
{
  int i,tmp,cf_num=3;
  double fit[3];
  int G[3],G_nx[3];
  double Gp[3]={0.4,0.4,0.2};

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
  bent_cigar_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0);
  i=1;
  hgbat_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0);
  i=2;
  rastrigin_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0);

  f[0]=0.0;
  for(i=0;i<cf_num;i++)
  {
    f[0] += fit[i];
  }
}

void hf10 (double *x, double *f, int nx, double *Os,double *Mr,int *S,int s_flag,int r_flag) /* Hybrid Function 6 */
{
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
  schaffer_F7_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0);

  f[0]=0.0;
  for(i=0;i<cf_num;i++)
  {
    f[0] += fit[i];
  }
}

void hf06 (double *x, double *f, int nx, double *Os,double *Mr,int *S,int s_flag,int r_flag) /* Hybrid Function 6 */
{
  int i,tmp,cf_num=5;
  double fit[5];
  int G[5],G_nx[5];
  double Gp[5]={0.3,0.2,0.2,0.1,0.2};

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
  katsuura_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0);
  i=1;
  happycat_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0);
  i=2;
  grie_rosen_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0);
  i=3;
  schwefel_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0);
  i=4;
  ackley_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0);
  f[0]=0.0;
  for(i=0;i<cf_num;i++)
  {
    f[0] += fit[i];
  }
}

void cf01 (double *x, double *f, int nx, double *Os,double *Mr,int r_flag) /* Composition Function 1 */
{
  int i,cf_num=5;
  double fit[5];
  double delta[5] = {10, 20, 30, 40, 50};
  double bias[5] = {0, 200, 300, 100, 400};

  i=0;
  rosenbrock_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag);
  fit[i]=10000*fit[i]/1e+4;
  i=1;
  ellips_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,0);
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

void cf02 (double *x, double *f, int nx, double *Os,double *Mr,int r_flag) /* Composition Function 2 */
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


void cf06 (double *x, double *f, int nx, double *Os,double *Mr,int r_flag) /* Composition Function 4 */
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

void cf07 (double *x, double *f, int nx, double *Os,double *Mr,int r_flag) /* Composition Function 4 */
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




void shiftfunc (double *x, double *xshift, int nx,double *Os)
{
  int i;
    for (i=0; i<nx; i++)
    {
        xshift[i]=x[i]-Os[i];
    }
}

void rotatefunc (double *x, double *xrot, int nx,double *Mr)
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

void sr_func (double *x, double *sr_x, int nx, double *Os,double *Mr, double sh_rate, int s_flag,int r_flag) /* shift and rotate */
{
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
}

void asyfunc (double *x, double *xasy, int nx, double beta)
{
  int i;
    for (i=0; i<nx; i++)
    {
    if (x[i]>0)
        xasy[i]=pow(x[i],1.0+beta*i/(nx-1)*pow(x[i],0.5));
    }
}

void oszfunc (double *x, double *xosz, int nx)
{
  int i,sx;
  double c1,c2,xx;
    for (i=0; i<nx; i++)
    {
    if (i==0||i==nx-1)
        {
      if (x[i]!=0)
        xx=log(fabs(x[i]));
      if (x[i]>0)
      {
        c1=10;
        c2=7.9;
      }
      else
      {
        c1=5.5;
        c2=3.1;
      }
      if (x[i]>0)
        sx=1;
      else if (x[i]==0)
        sx=0;
      else
        sx=-1;
      xosz[i]=sx*exp(xx+0.049*(sin(c1*xx)+sin(c2*xx)));
    }
    else
      xosz[i]=x[i];
    }
}


void cf_cal(double *x, double *f, int nx, double *Os,double * delta,double * bias,double * fit, int cf_num)
{
  int i,j;
  double *w;
  double w_max=0,w_sum=0;
  w = static_cast<double*>(malloc(cf_num * sizeof(double))); // Re-allocation for w
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
    f[0]=f[0]+w[i]/w_sum*fit[i];
    }
  free(w);
}

int main(void)
{
  std::vector<std::vector<double>> data =
  {
    {1, 2, 5, 3, 22, 2, 2, 7, 6, 5, 5, 4, 34, 1, 2, -13, 4, 5, 6, 7},
    {1, 2, 5, 3, 22, 44, 9, 7, 6, 5, 5, 4, 34, 1, 2, 3, 4, 5, 6, 7},
    {1, 2, 5, 3, 22, 44, 9, 71, 6, 3, 5, 4, 34, 1, -2, 3, 4, 5, 6, 7},
    {1, 2, 5, 3, 22, 44, -9, 7, 6, -12},
    {1, 2, 5, 3, 22, 44, 9, 7, 6, 5},
    {1, 2, 55, 3, 1, -14, 9, 70, 55, 1},
    {-10, 21},
    {0, -22},
    {2, 31}
  };

  for (int func_num = 1; func_num <= 12; ++func_num)
  {
    for (auto &instance_input : data)
    {
      int nx = instance_input.size();
      if (nx == 2 && (func_num == 6 || func_num == 7 || func_num == 8)) { continue; }

      double* f = static_cast<double*>(malloc(sizeof(double)));
      cec22_test_func(instance_input.data(), f, nx, 1, func_num);

      std::stringstream ss;
      ss << "EXAMPLE: " << func_num << " 1 ";
      for (double val : instance_input)
      {
        ss << val << ",";
      }
      std::string s = ss.str();
      s.pop_back();
      s += " " + std::to_string(f[0]);
      LOG(s);
    }
  }

  return 0;
}
