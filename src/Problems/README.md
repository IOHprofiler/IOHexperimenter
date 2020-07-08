## Problems of IOHexperimenter

[IOHprofiler_problem](/src/Template/IOHprofiler_problem.hpp) is the base `class` of problems of __IOHexperimenter__. The property variables of problems include:
* `problem_id` (optional), will be assigned if the problem is added to a suite, otherwise default by 0.
* `instance_id` (optional),  sets transformation methods on problems. The original problem is with instance_id 1, <i>scale</i> and <i>shift</i> are applied on objectives for instance_id in [2,100], <i>XOR</i> is applied on variables for instance_id in [2,50], and <i>sigma</i> function is applied on variables for instance_id in [51,100].
* `problem_name`
* `problem_type` (optional)
* `lowerbound`, is a vector of lowerbound for variables.
* `upperbound`, is a vector of upperbound for variables.
* `number_of_variables` (optional), is the dimension of the problem.
* `number_of_objectives`, is only available as 1 now. The functionality of multi-objectives is under development.
* `best_variables` (optional), is a vector of optimal solution, which is used to calculate the optimum. If both best_variables and optimum are not given, the optimum will be set as `std::numeric_limits<double>::max()` for maximization optimization, and as `std::numeric_limits<double>::lowest()` for minimization optimization.
* `optimal` (optional), is a vector of optimal objectives, but currently only single objective is supported. If both best_variables and optimum are not given, the optimum will be set as `std::numeric_limits<double>::max()` for maximization optimization, and as `std::numeric_limits<double>::lowest()` for minimization optimization.
* `maximization_minimization_flag`, sets as 1 for maximization, otherwise for minimization.

### Creating a problem
To create a problem of __IOHexperimenter__, the correct `IOHprofiler_problem<T>` needs to be inherited, and two functions need to be implemented: <i>construct functions</i> and <i>internel_evaluate</i>. Additionally, you can add pre-processing codes of allocating a problem in the virtual <i>prepare_problem</i> function, to make evluate process more efficient.

Taking the implementation of __OneMax__ with reduction transformation as an example, <i>construct functions</i> are as below. `problem_name` and `number_of_objectives` __must__ be set.
```cpp
OneMax_Dummy1(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
  IOHprofiler_set_instance_id(instance_id);
  IOHprofiler_set_problem_name("OneMax_Dummy1");
  IOHprofiler_set_problem_type("pseudo_Boolean_problem");
  IOHprofiler_set_number_of_objectives(1);
  IOHprofiler_set_lowerbound(0);
  IOHprofiler_set_upperbound(1);
  IOHprofiler_set_best_variables(1);
  IOHprofiler_set_number_of_variables(dimension);
}

~OneMax_Dummy1() {};
```

The <i>internal_evaluate</i> __must__ be implemented as well. It is used during evaluate process, returning a (real) objective values of the corresponding variables __x__. In this case, the evaluate function applies a variable `info`. To avoid wasting time on calculating `info` within <i>internal_evaluate</i> for each evaluation, `info` is prepared in the <i>prepare_problem</i> function.
```cpp
std::vector<int> info;
void prepare_problem() {
  info = dummy(IOHprofiler_get_number_of_variables(),0.5,10000);
}

double internal_evaluate(const std::vector<int> &x) {
  int n = this->info.size();
  int result = 0;
  for (int i = 0; i != n; ++i) {
    result += x[this->info[i]];
  }
  return (double)result;
};
```

If you want to register your problem using `problem_name` and add it into a suite, please add functions for creating instances as following codes.

```cpp
static OneMax_Dummy1 * createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
    return new OneMax_Dummy1(instance_id, dimension);
  };
};
```

To register the problem, you can use the <i>geniricGenerator</i> in [IOHprofiler_class_generator](https://github.com/IOHprofiler/IOHexperimenter/blob/developing/src/Template/IOHprofiler_class_generator.hpp). For example, you can use the following statement to register and create __OneMax__ with reduction transformation,

```cpp
// Register
static registerInFactory<IOHprofiler_problem<int>,OneMax_Dummy1> regOneMax_Dummy1("OneMax_Dummy1");
// Create
std::shared_ptr<IOHprofiler_problem<int>> problem = genericGenerator<IOHprofiler_problem<int>>::instance().create("OneMax_Dummy1");
```