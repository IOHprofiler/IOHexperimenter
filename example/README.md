## Using IOHexperimenter in C++

__IOHexperimenter__ consists of three main compenents: problems, suites, and loggers.

<a name="problems"></a>
## Testing Problems 

Users can test algorithms on specific __Problem__ classes. Twenty-five pseudo-Boolean optimization problems, twenty-four bbob problems, and two classes of W-Model problems are available in __IOHexperimenter__.

An example of testing the random search on __OneMax__ is provided in [problem_example.h](problem_example.h). A problem class is initiated with the given instance and dimension. The following statement declares a pointer of the instance 1 of `OneMax` in dimension 10, which belongs to the `pbo` space.
```cpp
const auto om = std::make_shared<ioh::problem::pbo::OneMax>(1, 10);
```
Similarly, the following statement declares a pointer of the instance 1 of `Sphere` in dimension 5, which belongs to the `bbob` space. 
```cpp
const auto sp = std::make_shared<ioh::problem::bbob::Sphere>(1, 5);
```

The `meta_data()` class provides access to useful information of the problem. The following statement can output the information of the `OneMax` class.
```cpp
std::cout << om->meta_data() << std::endl;
```

To obtain the fitness of `x` on the `OneMax`, we can easily use the statement.
```cpp
auto y = (*om)(x).at(0)
```

## Testing Suites

Suites are collections of problems. Two suites, PBO for the pseudo-Boolean problems and BBOB for the continuous problems, are available in __IOHexperimenter__. An example of using suites is provided in [suite_example.h](suite_example.h).

The following statements create a `bbob` consisting of 8 problems of the BBOB suite:
1. problem 1, instance 1, dimension 5
2. problem 1, instance 3, dimension 5
3. problem 1, instance 1, dimension 10
4. problem 1, instance 3, dimension 10
5. problem 2, instance 1, dimension 5
6. problem 2, instance 3, dimension 5
7. problem 2, instance 1, dimension 10
8. problem 2, instance 3, dimension 10

```cpp
const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();
const auto bbob = suite_factory.create("BBOB", {1, 2}, {1, 3}, {5, 10});
```

In the given example, a random search is applied across all problems within a loop :
```cpp
for (const auto &problem : *bbob) :
{
  ...
}
```
The variable `problem` is a pointer of __Problem__ class upper introduced.


## Using Logger

A default logger is provided to record evaluation information during the optimization process into csv files. The following statement declares a logger, which outputs files into a folder named as `logger_example` at the directory `./`. The algorithm name `random_search` and the algorithm information `a random search for ...` will be attached into the csv files.
```cpp
auto l = ioh::logger::Default("./", "logger_example", "random_search", "a random search for testing the bbob suite");
```

By default, evaluations when the best-found fitness is updated will be recorded into csv files. This can be changed by passing arguments to the constructor. In practice,

`t_always = true` turns on the .cdat files, which record all evaluations.

`t_on_interval = 0` turns off the .idat files. If a positive value i is assigned, evaluations will be recorded in .idat files every i times,

`t_on_improvement = true` turns on the .dat files, which record evaluations when the best-found fitness is updated,

`t_per_time_range = tp` and `t_at_time_points = ta` control the .tdat files together. The .tdat are turned off by default. Please find details in the [Sec 3. of the document](https://arxiv.org/pdf/1810.05281.pdf).

The logger shall be attached to a __Problem__ class at the beginning of the experiment for a problem.
```cpp
problem->attach_logger(l);
```

## Using Experiment

__Experiment__ class automatically test the given __solver__ on the pre-defined __suite__ of problems and record the optimization process using a __logger__. The argument of a solver must be a pointer of IOH __Problem__, and the type of the __Problem__ must be consistent with the suite.
```cpp
// using ioh::problem::Integer for discrete optimization.
void solver(const std::shared_ptr<ioh::problem::Real> p)
{
  ...
}
```

The following statements provide an example of testing the `solver` on a bbob suite, the `solver` performs 10 independent runs on each instance, and csv files will be stored in a folder `logger-experimenter`,
```cpp
const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();
const auto suite = suite_factory.create("BBOB", {1, 2}, {1, 2}, {5, 10});
const auto logger = std::make_shared<ioh::logger::Default>(std::string("logger-experimenter"));

ioh::experiment::Experimenter<ioh::problem::Real> f(suite, logger, solver, 10);
f.run();
```
