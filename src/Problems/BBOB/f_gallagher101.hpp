/// \file f_gallagher101.hpp
/// \brief cpp file for class f_gallagher101.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_gallagher.c"
///
/// \author Furong Ye
/// \date 2019-09-12
#ifndef _F_GALLAGHERONEZEROONE_H
#define _F_GALLAGHERONEZEROONE_H

#include "../../Template/IOHprofiler_problem.hpp"
#include "bbob_common_used_functions/coco_transformation.h"

typedef struct f_gallagher_permutation_t1{
  double value;
  size_t index;
};

static bool compareRperm1(f_gallagher_permutation_t1 a, f_gallagher_permutation_t1 b) {
  double temp = a.value - b.value;
  if (temp >= 0)
    return true;
  else
    return false;
};

class Gallagher101 : public IOHprofiler_problem<double> {
public:
  Gallagher101() {
    IOHprofiler_set_problem_name("Gallagher101");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    IOHprofiler_set_as_minimization();
  }
  Gallagher101(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Gallagher101");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    Initilize_problem(dimension);
    IOHprofiler_set_as_minimization();
  }
  ~Gallagher101() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  std::vector<double> xopt;
  const size_t number_of_peaks = 101;
  std::vector<std::vector<double> > rotation;
  std::vector<std::vector<double>> arr_scales;
  std::vector<std::vector<double>> x_local;
  std::vector<double> peak_values;
  void prepare_problem() {
    double fopt;
    const long rseed = (long) (21 + 10000 * this->IOHprofiler_get_instance_id());
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    bbob2009_compute_xopt(xopt, rseed, n);
    fopt = bbob2009_compute_fopt(21, this->IOHprofiler_get_instance_id());
    std::vector<double> tmp_best_variables(n,0.0);
    /* compute rotation */
    rotation = std::vector<std::vector<double>> (n);
    for (int i = 0; i != n; i++) {
      rotation[i] = std::vector<double> (n);
    }
    bbob2009_compute_rotation(rotation,rseed,n);
    size_t i, j, k;
    double maxcondition = 1000.;
    /* maxcondition1 satisfies the old code and the doc but seems wrong in that it is, with very high
    * probability, not the largest condition level!!! */
    double maxcondition1 = 1000.;
    
    double fitvalues[2] = { 1.1, 9.1 };
    /* Parameters for generating local optima. In the old code, they are different in f21 and f22 */
    double b, c;
    /* Random permutation */
    std::vector<double> random_numbers(number_of_peaks * n);

  
    maxcondition1 = sqrt(maxcondition1);
    b = 10.;
    c = 5.;

    /* Initialize all the data of the inner problem */
    bbob2009_unif(random_numbers, number_of_peaks - 1, rseed);
    
    std::vector<f_gallagher_permutation_t1> rperm;
    for (i = 0; i < number_of_peaks - 1; ++i) {
      f_gallagher_permutation_t1 temp_rperm;
      temp_rperm.value = random_numbers[i];
      temp_rperm.index = i;
      rperm.push_back(temp_rperm);
    }

    
    std::sort(rperm.begin(), rperm.end(),compareRperm1);

    /* Random permutation */
    std::vector<double> arrCondition(number_of_peaks);
    arrCondition[0] = maxcondition1;
    peak_values = std::vector<double>(number_of_peaks);
    peak_values[0] = 10;
    for (i = 1; i < number_of_peaks; ++i) {
      arrCondition[i] = pow(maxcondition, (double) (rperm[i - 1].index) / ((double) (number_of_peaks - 2)));
      peak_values[i] = (double) (i - 1) / (double) (number_of_peaks - 2) * (fitvalues[1] - fitvalues[0])
          + fitvalues[0];
    }

    rperm.clear();
    for (i = 0; i < n; ++i) {
      f_gallagher_permutation_t1 temp_rperm;
      rperm.push_back(temp_rperm);
    }

    arr_scales = std::vector<std::vector<double> > (number_of_peaks);
    for (int i = 0; i != number_of_peaks; i++) {
      arr_scales[i] = std::vector<double> (n);
    }
    for (i = 0; i < number_of_peaks; ++i) {
      bbob2009_unif(random_numbers, n, rseed + (long) (1000 * i));
      for (j = 0; j < n; ++j) {
        rperm[j].value = random_numbers[j];
        rperm[j].index = j;
      }
      sort(rperm.begin(), rperm.end(),compareRperm1);
      for (j = 0; j < n; ++j) {
        arr_scales[i][j] = pow(arrCondition[i],((double) rperm[j].index) / ((double) (n - 1)) - 0.5);
      }
    }

    x_local = std::vector<std::vector<double> > (n);
    for (int i = 0; i != n; i++) {
      x_local[i] = std::vector<double> (number_of_peaks);
    }
    bbob2009_unif(random_numbers, n * number_of_peaks, rseed);
    for (i = 0; i < n; ++i) {
      xopt[i] = 0.8 * (b * random_numbers[i] - c);
      tmp_best_variables[i] = 0.8 * (b * random_numbers[i] - c);
      for (j = 0; j < number_of_peaks; ++j) {
        x_local[i][j] = 0.;
        for (k = 0; k < n; ++k) {
          x_local[i][j] += rotation[i][k] * (b * random_numbers[j * n + k] - c);
        }
        if (j == 0) {
          x_local[i][j] *= 0.8;
        }
      }
    }
    Coco_Transformation_Data::fopt = fopt;
    IOHprofiler_set_best_variables(tmp_best_variables);
  }


  double internal_evaluate(const std::vector<double> &x) {
    int n = x.size();

    size_t i, j; /* Loop over dim */
    std::vector<double> tmx(n);
    double a = 0.1;
    double tmp2, f = 0., f_add, tmp, f_pen = 0., f_true = 0.;
    double fac;
    std::vector<double> result(1);


    fac = -0.5 / (double) n;

    /* Boundary handling */
    for (i = 0; i < n; ++i) {
      tmp = fabs(x[i]) - 5.;
      if (tmp > 0.) {
        f_pen += tmp * tmp;
      }
    }
    f_add = f_pen;
    /* Transformation in search space */
    /* TODO: this should rather be done in f_gallagher */
    
    for (i = 0; i < n; i++) {
      tmx[i] = 0;
      for (j = 0; j < n; ++j) {
        tmx[i] += rotation[i][j] * x[j];
      }
    }
    /* Computation core*/
    for (i = 0; i < n; ++i) {
      tmp2 = 0.;
      for (j = 0; j < n; ++j) {
        tmp = (tmx[j] - x_local[j][i]);
        tmp2 += arr_scales[i][j] * tmp * tmp;
      }
      tmp2 = peak_values[i] * exp(fac * tmp2);
      f = f > tmp2 ? f : tmp2;
    }

    f = 10. - f;
    if (f > 0) {
      f_true = log(f) / a;
      f_true = pow(exp(f_true + 0.49 * (sin(f_true) + sin(0.79 * f_true))), a);
    } else if (f < 0) {
      f_true = log(-f) / a;
      f_true = -pow(exp(f_true + 0.49 * (sin(0.55 * f_true) + sin(0.31 * f_true))), a);
    } else {
      f_true = f;
    }

    f_true *= f_true;
    f_true += f_add;
    result[0] = f_true;
   
    return result[0];
  };
  
  static Gallagher101 * createInstance() {
    return new Gallagher101();
  };

  static Gallagher101 * createInstance(int instance_id, int dimension) {
    return new Gallagher101(instance_id, dimension);
  };
};

#endif