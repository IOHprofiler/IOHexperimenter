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

# Usage guide for IOHexperimenter in python

This document describes how to use the python version of the IOHexperimenter.
Note that these instructions are only for versions >= 1.0.0

## Installation

You can install the experimenter directly from pip:

```bash
pip3 install IOHexperimenter
```

To verify the installation, you can use:

```bash
pip3 freeze | grep IOHexperimenter
```

## Create a function object

The most basic way to use the IOHexperimenter is to just access the problems from our predefined problem suites. Currently, we have implemented 25 Pseudo-Boolean problems and the single-objective BBOB-problems from COCO.

To use these problems, you can import the associated module as follows:

```python
from ioh import problem
```

For all of these problems, you can then access the docstrings to get more information about precise usage. 
```python
?problem
```

These modules can then be used exactly as their c++ equivalent. However, for convenience, some wrapper functions are provided, such as the get_problem function.
To get a 5-dimensional sphere function, with instance number 1, you can use the following:

```python
# C++ style
f = problem.Real.factory().create("Sphere", 1, 5)

# Using wrapper
from ioh import get_problem
f = get_problem("Sphere", 1, 5)
```
Instead of using the name of the function, you can also use their function number within their respecitve suite:

```python
#Create a problem object from the Integer class (PBO functions) by fid, iid, dim
f1 = get_problem(7, 1, 5, suite="PBO")
```
With these problem-objects, the state, meta_data and contrainsts can easily be accessed:

```python
f.meta_data.n_variables
```

```python
f.constraint.lb, f.constraint.ub
```

```python
f.state.final_target_hit
```

And the problem can be evaluated by calling it:

```python
f([0,0,0,0,0])
```

This will then immediately update the state-variables:

```python
f.state.evaluations
```

## Running an algorithm

To showcase how to use these functions in combination with an optimization algorith, we define a simple random-search example wich accepts an argument of class IOH_function.


```python
import numpy as np
np.random.seed(42)
```

```python
def random_search(func, budget = None):
    if budget is None:
        budget = int(func.meta_data.n_variables * 1e3)

    for i in range(budget):
        x = np.random.uniform(func.constraint.lb, func.constraint.ub)
        func(x)
```

To record data, we need to add a logger to the problem. We can do this easily by importing the logger module:

```python
from ioh import logger
?logger.Default
```

This logger can then be initialized with the directory in which to store the logs:

```python
#Create default logger which writes to folder 'temp'
l = logger.Default("temp")
```
This can then be attached to the problem:

```python
f = get_problem(1, 1, 5, suite="BBOB")
```

```python
f.attach_logger(l)
```

Now, we can run the algorithm and store data


```python
random_search(f)
```

To ensure all data is written, we should flush the logger after running our experiments


```python
l.flush()
```

### Tracking parameters

If we want to track parameters of the algorithm, we need to slightly restructure it by turning it into a class, where the variables we want to track are properties

```python
class opt_alg:
    def __init__(self, budget):
        #Note that we should re-initialize all dynamic variables if we want to run the same algorithm multiple times
        self.budget = budget

    def __call__(self, func):
        self.f_opt = np.Inf
        self.x_opt = None
        for i in range(self.budget):
            x = np.random.uniform(func.lowerbound, func.upperbound)
            f = func(x)
            if f < self.f_opt:
                self.f_opt = f
                self.x_opt = x
        return self.f_opt, self.x_opt
    
    @property
    def param_rate(self):
        return np.random.randint(100)
```

We could then use the function track_parameters of the logger to register the parameter to be tracked


```python
?l.track_parameters
```

Alternatively, we can use the IOHexperimenter class to easily run the benchmarking in multiple functions

```python
from ioh import experimenter, suite
```


```python
?experimenter
```

This can be initialized using a suite (PBO or BBOB are available) by providing lists of function ids (or names), dimensions, instance ids and a number of independent repetitions as follows:


```python
s = suite.BBOB(list(range(1, 3)), [5, 10], [5])
```


```python
l = logger.Default("data", store_positions=True)

e = experimenter.Real(s, l, opt_alg, 5)
```


```python
e.run()
```
To start analyzing this data, simply zip the 'data'-folder and upload it to the IOHanalyzer!

