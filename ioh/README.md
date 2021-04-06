
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

