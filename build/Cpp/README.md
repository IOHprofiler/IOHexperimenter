## Using IOHexperimenter in C++

There are three main ways to use __IOHexperimenter__ benchmark algorithms:
* [Test on individual problems](#problems)
* [Test on suites, which are predefined collections of problems](#suites)
* [Test using an experiment with a configuration file (__recommended__)](#experimenter)

<a name="problems"></a>
## Test on individual problems

To use __IOHexperimenter__ to run benchmarking on a specific problem, the template-file `IOHprofiler_run_problem.cpp` is provided. Since all problems within the __IOHexperimenter__ are defined as specific derived `class` inheriting problem `IOHprofiler_problem` class, it is quite straightforward to use them. 


<!-- , the source codes are available in the [Problems folder](https://github.com/IOHprofiler/IOHexperimenter/src/Problems). For the definition of the problems already implemented in IOHexperimenter, please visit the wiki page [https://iohprofiler.github.io/Benchmark/Problems/](https://iohprofiler.github.io/Benchmark/Problems/). -->

An example testing evolutionary algorithm with mutation operator on __OneMax__ is implemented in `IOHprofiler_run_problem.cpp`. To use a different function, modify the include-statement to include the problem to use, and use the corresponding class-name instead of __OneMax__.

For this example, a `OneMax` class is declared and initialized with dimension 1000 on the default instance of the probelem.
```cpp
OneMax om;
int dimension = 1000;
om.Initilize_problem(dimension);
```

During the optimization process, the algorithm can acquire the fitness value through <i>evaluate()</i> function. In the example below, <i>om.evaluate(x)</i> returns the fitness of `x`. Another option is the statement <i>om.evaluate(x,y)</i>, which stores the fitness of `x` in `y`. `logger` is an __IOHprofiler_csv_logger__ class, which stores function evaluations in a format compatible with __IOHanalyzer__. <i>logger.write_line(om.loggerInfo())</i> deliveries the lastest information of tested `om` to the `logger`.  In addition, <i>om.IOHprofiler_hit_optimal()</i> is an indicator you can use to check if the optimum has been found.
```cpp
while (!om.IOHprofiler_hit_optimal()) {
  x = x_star;
  if (mutation(x, mutation_rate)) {
    y = om.evaluate(x);
    logger.write_line(om.loggerInfo());
  }
  if (y[0] > best_value) {
    best_value = y;
    x_star = x;
  }
}
```

If, for your experiment, you want to generate data to be used in the __IOHanalyzer__, a `IOHprofiler_csv_logger` should be added to the problem you are testing on. The arguments of `IOHprofiler_csv_logger` are directory of result folder, name of result folder, name of the algorithm and infomation of the algorithm. With different setting of triggers (observer), mutilple data files are to be generated for each experiment. More details on the available triggers are available [here](/IOHexperimenter/Loggers/Observer). Before optimizing a problem, `logger` must be targeted with the problem using the statement <i>logger.target_problem()</i>, with which arguments are problem id, dimension, instance id, problem name, and the type of optimization (maximization or minimization).

```cpp
std::vector<int> time_points{1,2,5};
std::shared_ptr<IOHprofiler_csv_logger> logger(new IOHprofiler_csv_logger("./","run_problem","EA","EA"));
logger->set_complete_flag(true);
logger->set_interval(0);
logger->set_time_points(time_points,10);
logger->activate_logger();
logger.target_problem(om.IOHprofiler_get_problem_id(), 
                      om.IOHprofiler_get_number_of_variables(), 
                      om.IOHprofiler_get_instance_id(),
                      om.IOHprofiler_get_problem_name(),
                      om.IOHprofiler_get_optimization_type());
```

<a name="suites"></a>
## Test on suites
Suites are collections of test problems. The idea behind a suite is that packing problems with similar properties toghther makes it easier to test algorithms on a class of problems. Currently, two pre-defined suites are available: [__PBO__](Benchmark/), consisting of 23 __pseudo Boolean problems__, and [__BBOB__](https://coco.gforge.inria.fr/downloads/download16.00/bbobdocfunctions.pdf), consisting of 24 __real-valued problems__. To find out how to create your own suites, please visit [this page](/IOHexperimenter/Adding-Functions/).

An example of testing an evolutionary algorithm with mutation operator on  the __PBO__ suite is implemented in `IOHprofiler_run_suite.cpp`. __PBO__ suite includes pointers to 23 problems. To instantiate problems you want to test, the vectors of problem id, instances and dimensions need to be given as follows:
```cpp
std::vector<int> problem_id = {1,2};
std::vector<int> instance_id ={1,2};
std::vector<int> dimension = {100,200,300};
PBO_suite pbo(problem_id,instance_id,dimension);
```

With the suite, you can test problems of the suite one by one, until all problems have been tested. In this example, the order of problem is as follow, and an `evlutionary_algorithm` is applied:

1. problem id 1, instance 1, dimension 100
2. problem id 1, instance 2, dimension 100
3. problem id 1, instance 1, dimension 200
4. problem id 1, instance 2, dimension 200
5. problem id 1, instance 1, dimension 300
6. problem id 1, instance 2, dimension 300
7. problem id 2, instance 1, dimension 100
8. problem id 2, instance 2, dimension 100
9. problem id 2, instance 1, dimension 200
10. problem id 2, instance 2, dimension 200
11. problem id 2, instance 1, dimension 300
12. problem id 2, instance 2, dimension 300

```cpp
while (problem = pbo.get_next_problem()) {
  evolutionary_algorithm(problem);
}
```

If, for your experiment, you want to generate data to be used in the __IOHanalyzer__, a `IOHprofiler_csv_logger` should be added to the suite. The arguments of `IOHprofiler_csv_logger` are the directory of result folder, name of result folder, name of the algorithm and infomation of the algorithm. In addition, you can set up mutilple triggers of recording evaluations. For the details of triggers, please visit the introduction of [IOHprofiler_observer](/IOHexperimenter/Loggers/).
```cpp
std::vector<int> time_points{1,2,5};
std::shared_ptr<IOHprofiler_csv_logger> logger(new IOHprofiler_csv_logger("./","run_suite","EA","EA"));
logger->set_complete_flag(true);
logger->set_interval(2);
logger->set_time_points(time_points,3);
logger->activate_logger();
logger->target_suite(pbo.IOHprofiler_suite_get_suite_name());
```

<a name="experimenter"></a>
## Test using an experiment with a configuration file

By using the provided `IOHprofiler_experiment` class, you can use a configuration file to configure both the suite and the logger for csv files.

To use the provided experiment structure, you need to provide both the path to the configuration file and the pointer to your optimization algorithm to the <i>experimenter._run()</i> function, which will execute all tasks of the experiment.

In addition, you can set the number of repetitions for all problems by using <i>experimenter._set_independent_runs(2)</i>.
```cpp
std::string configName = "./configuration.ini";
IOHprofiler_experimenter<int> experimenter(configName,evolutionary_algorithm);
experimenter._set_independent_runs(2);
experimenter._run();
```

For the content of configuration file, it consists of three sections:

__suite__ configures the problems to be tested.
* __suite_name__, is the name of the suite to be used. Please make sure that the suite with the corresponding name is registered.
* __problem_id__, configures problems to be tested. Note that id of problems are configured by the suite, please make sure that id is within the valid range.
* __instance_id__, configures the transformation methods applied on problems. 
  For `PBO`:
  * `1` : no transformer operations on the problem.
  * `2-50` :  XOR and SHIFT operations are applied on the problem.
  * `51-100`: SIGMA and SHIFT operations are applied on the problem.
* __dimension__, configures dimension of problems to be tested. Note that allowed dimension is not larger than 20000.

__logger__ configures the setting of output csv files.
* __result_foler__ is the directory of the folder where sotres output files.
* __algorithm_name__, is the name of the algorithm, which is used when generating '.info' files.
* __algorithm_info__, is additional information of the algorithm, which is used when generating '.info' files.

__observer__ configures parameters of `IOHprofiler_server`, which is used in `IOHprofiler_csv_logger`,
* __complete_triggers__ is the switch of `.cdat` files, which works with __complete tracking__ strategy. Set it as `TRUE` or `true` if you want to output `.cdat` files.
* __update_triggers__ is the switch of `.dat` files, which works with __target-based strategy__ strategy. Set it as `TRUE` or `true` if you want to output <i>.dat</i>` files.
* __number_interval_triggers__ configures the `.idat` files, which works with __interval tracking__  number_target_triggers sets the value of the frequecny. If you do not want to generate `.idat` files, set `number_target_triggers` as 0.
* __number_target_triggers__ configures the `.tdat` files, which works with __time-based tracking__ strategy.
* __base_evaluation_triggers__ configures the `.tdat` files, which works with __time-based tracking__ strategy. To switch off `.tdat` files, set both __number_target_triggers__ and __base_evaluation_triggers__ as 0.

<a name="memberfunctions"></a>
## Useful functions
`IOHprofiler_problem` and `IOHprofiler_suite` provide public member functions so that the optimizer can acquire useful information during optimization process.

A list of useful member functions of `IOHprofiler_problem` is below:
* <i>evaluate(x)</i>, returns a fitness values. The argument __x__ is a vector of variables.
* <i>evaluate(x,y)</i>, updates __y__ with a fitness values, and __x__ is a vector of variables.
* <i>reset_problem()</i>, reset the history information of problem evaluations. You should call this function at first when you plan to do another test on the same problem class.
* <i>IOHprofiler_hit_optimal()</i>, returns true if the optimum of the problem has been found.
* <i>IOHprofiler_get_number_of_variables(number_of_variables)</i>, returns dimension of the problem.
* <i>IOHprofiler_get_evaluations()</i>, returns the number of function evaluations that has been used.
* <i>loggerInfo</i>, returns a vector of information of function evaluations, which consists of evaluations, current found raw objective, best so far found raw objective, current found transformed objective, and best of far best found transformed objective.
* <i>loggerCOCOInfo</i>, returns a vector of information of function evaluations, which consists of evaluations, precision of current found objective, best so far found precision, current found objective, and best so far found objective.
* <i>IOHprofiler_get_problem_id()</i>
* <i>IOHprofiler_get_instance_id()</i>
* <i>IOHprofiler_get_problem_name()</i>
* <i>IOHprofiler_get_problem_type()</i>
* <i>IOHprofiler_get_lowerbound()</i>
* <i>IOHprofiler_get_upperbound()</i>
* <i>IOHprofiler_get_number_of_objectives()</i>
* <i>IOHprofiler_get_optimization_type()</i>

A list of useful member functions of `IOHprofiler_suite` is below:
* <i>get_next_problem</i>, return a shared point of problems of the suite in order.
* <i>get_current_problem()</i>, returns current problem and reset it.
* <i>get_problem(problem_name,instance,dimension)</i>, returns the specific problem.
* <i>IOHprofiler_suite_get_number_of_problems</i>
* <i>IOHprofiler_suite_get_number_of_instances</i>
* <i>IOHprofiler_suite_get_number_of_dimensions</i>
* <i>IOHprofiler_suite_get_problem_id</i>
* <i>IOHprofiler_suite_get_instance_id()</i>
* <i>IOHprofiler_suite_get_dimension()</i>
* <i>IOHprofiler_suite_get_suite_name()</i>