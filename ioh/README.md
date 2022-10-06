## Quickstart

Note that these instructions are for versions >= 0.3.2.4.

### Installation

You can install the experimenter directly from pip:

```bash
pip install ioh>=0.3.2.4
```

To verify the installation, you can use:

```bash
pip show ioh
```

### Create a function object

The most basic way to use the IOHexperimenter is to just access the problems from our predefined problem suites. Currently, we have implemented 25 Pseudo-Boolean problems and the single-objective BBOB-problems from COCO.

To use these problems, you can import the associated module as follows:

```python
from ioh import problem
```

For all of these problems, you can then access the docstrings to get more information about precise usage.

```python
help(problem)
```

These modules can then be used exactly as their c++ equivalent. However, for convenience, some wrapper functions are provided, such as the get_problem function.
To get a 5-dimensional sphere function, with instance number 1, you can use the following:

```python
# C++ style
f = problem.Real.create("Sphere", 1, 5)

# Using wrapper
from ioh import get_problem
f = get_problem("Sphere", 1, 5)
```

Instead of using the name of the function, you can also use their function number within their respecitve suite:

```python
#Create a problem object from the Integer class (PBO functions) by fid, iid, dim
f1 = get_problem(7, 1, 5, problem_type="PBO")
```

With these problem-objects, the state, meta_data and contrainsts can easily be accessed:

```python
f1.meta_data
```

```python
f1.bounds
```

```python
f1.state
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
        x = np.random.uniform(func.bounds.lb, func.bounds.ub)
        func(x)
```

To record data, we need to add a logger to the problem. We can do this easily by importing the logger module:

```python
from ioh import logger
help(logger.Analyzer)
```

This logger can then be initialized with the directory in which to store the logs:

```python
#Create default logger which writes to folder 'temp'
l = logger.Analyzer(folder_name="temp")
```

This can then be attached to the problem:

```python
f = get_problem(1, 1, 5, problem_type="BBOB")
```

```python
f.attach_logger(l)
```

Now, we can run the algorithm and store data

```python
random_search(f)
```

To ensure all data is written, we should flush the logger after running our experiments. This happens automatically when the python process terminates, but when running in an interactive environment, such as a jupyter notebook, this should be done explicitly.

```python
f.reset()
l.close()
```

Note that we should call the `reset` method on a problem. By calling the `reset` method, the problem's internal state is automatically reset, and the data for the attached loggers will be flushed to the output streams. This function should also be used when performing multiple runs of the same problem in a loop, for example:

```python
f5 = get_problem(5, 1, 5)
l2 = logger.Analyzer(folder_name="temp2")
f5.attach_logger(l2)

for run in range(10):    
    random_search(f5)
    f5.reset()
```

### Tracking parameters

If we want to track parameters of the algorithm, these variables need to be aviable on a object, i.e. as member variables on a class instance. In order to do this for the random search algorithm, we can do this be wrapping the function in a class.

```python
class RandomSearch:
    def __init__(self, budget):
        #Note that we should re-initialize all dynamic variables if we want to run the same algorithm multiple times
        self.budget = budget
        
        # A parameter static over the course of an optimization run of an algorithm
        self.algorithm_id = np.random.randint(100) 
        
        # A dynamic parameter updated by the algorithm 
        self.a_tracked_parameter = None

    def __call__(self, func):
        self.f_opt = np.Inf
        self.x_opt = None
        for i in range(self.budget):
            x = np.random.uniform(func.bounds.lb, func.bounds.ub)
            
            # Update the tracked parameter
            self.a_tracked_parameter = i ** 10 
            
            f = func(x)
            if f < self.f_opt:
                self.f_opt = f
                self.x_opt = x
            
        return self.f_opt, self.x_opt
    
    @property
    def a_property(self):
        return np.random.randint(100)
        
    def reset(self):
        self.algorithm_id = np.random.randint(100) 
```

A full example is below, we first instantiate the algorithm instance, after which we create a new logger. Then we update the logger instance, telling it to track the `algorithm_id` parameter when we start a new run with a problem. The variable `a_tracked_parameter` is updated before every problem evaluation, so we want the logger to store this variable at every problem evaluation. We can also do this for properties, using the same method.

```python
algorithm = RandomSearch(10_000)
l3 = logger.Analyzer(folder_name="temp3")

# Track variables static over the course of an algorithm run
l3.add_run_attributes(algorithm, ["algorithm_id"])

# Track dynamic variables, updated during the run of an algorithm
l3.watch(algorithm, ["a_property", "a_tracked_parameter"])

# attach a problem to the logger
p2 = get_problem(2, 2, 5)
p2.attach_logger(l3)

for run in range(10):
    algorithm(p2)     # run the algorithm on the problem
    p2.reset()        # reset the problem and logger state  
    algorithm.reset() # update the algorithm_id parameter
```

Alternatively, we can use the `Experiment` class, to more easily run the benchmarking experiment with multiple functions.

```python
from ioh import Experiment
```

```python
help(Experiment)
```

This can be initialized using a suite (PBO or BBOB are available) by providing lists of function ids (or names), dimensions, instance ids and a number of independent repetitions as follows:

```python
exp = Experiment(
    RandomSearch(10_000),   # instance of optimization algorithm
    [1],                    # list of problem id's
    [1, 2],                 # list of problem instances
    [5],                    # list of problem dimensions
    problem_type = 'BBOB',  # the problem type, function ids should correspond to problems of this type
    njobs = 2,              # the number of parrellel jobs for running this experiment
    reps = 2,               # the number of repetitions for each (id x instance x dim)
    logged_attributes = [   # list of the tracked variables, must be available on the algorithm instance (RandomSearch)
        "a_property", 
        "a_tracked_parameter"
    ]                      
)
```

```python
exp.run()
```

To start analyzing this data, simply upload the zip 'data'-folder and upload it to the IOHanalyzer!
