## C++ Interface

### Installation

The following toolkits are needed for compiling IOHexperimenter:

* A `C++` compiler. The minimum compiler version is g++ 7 or equivalent, but we recommend g++ 9 or equivalent.
* [CMake](https://cmake.org), version 3.12 or higher

Please use the following commands to download, compile, and install this package:

```sh
> git clone --recursive https://github.com/IOHprofiler/IOHexperimenter.git
> cd IOHexperimenter
> mkdir build
> cd build
> cmake .. && make install
```

which installs all header files to `/usr/local/include/ioh` by default.
If you want to change this directory, please use the following flag `cmake -DCMAKE_INSTALL_PREFIX=your/path ..`

If you want to change build options, check the output of `cmake -L` or use `cmake-gui` or `ccmake`.

### Access IOH problem class

To obtain a built-in problem, you could create a problem instance by passing the
*instance id* and the *search dimension* to the constructor of the intended problem class, e.g.,

```C++
#include "ioh.hpp"
const auto om = std::make_shared<ioh::problem::pbo::OneMax>(1, 10); // PBO problem: instance 1, dim 10
const auto sp = std::make_shared<ioh::problem::bbob::Sphere>(1, 5); // BBOB problem: instance 1, dim 5
```

The instance id is intended to generalize a certain problem by some transformations, where
it serves as the random seed for randomizing the transformations, e.g., affine
transforms for BBOB problems and scaling of objective values for PBO problems. Please see

* [PBO transformations](https://iohprofiler.github.io/IOHproblem/)
* [BBOB/COCO transformations](https://coco.gforge.inria.fr/downloads/download16.00/bbobdocfunctions.pdf)

We also provide problem factories for this purpose:

```C++
const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::Integer>::instance();
const auto om = problem_factory.create("OneMax", 1, 10);
```

In addition, we provide suites for the [PBO](https://iohprofiler.github.io/IOHproblem/) and [BBOB/COCO](https://github.com/numbbo/coco) problems. To collect a set of problem, you can create a suite by passing the sets of *problem ids*, *instance ids*, and *search dimensions*, e.g.,

```C++
const std::vector<int> problems{1, 2};
const std::vector<int> instances{1, 2};
const std::vector<int> dimensions{1, 2};

The following statements create a `bbob` consisting of 8 problems of the BBOB suite:
  1. problem 1, instance 1, dimension 5
  2. problem 1, instance 3, dimension 5
  3. problem 1, instance 1, dimension 10
  4. problem 1, instance 3, dimension 10
  5. problem 2, instance 1, dimension 5
  6. problem 2, instance 3, dimension 5
  7. problem 2, instance 1, dimension 10
  8. problem 2, instance 3, dimension 10

We also provide factories for the suites:
```C++
const auto bbob_suite ioh::suite::SuiteRegistry<ioh::problem::Real>::instance().create("BBOB", problems, instances, dimensions);
```

Some complete examples to demonstrate the basic usage are also available:

* Using [a single problem](https://github.com/IOHprofiler/IOHexperimenter/blob/master/example/problem_example.h)
* Using a pre-defined [problem suite/set](https://github.com/IOHprofiler/IOHexperimenter/blob/master/example/suite_example.h)
* Using the [logging ability](https://github.com/IOHprofiler/IOHexperimenter/blob/master/example/logger_example.h) for storing benchmark data.

### Add new problems

We offer a very simple and convenient interface for integrating new benchmark problems/functions. First, you could define a new `test_problem` as you like. Note that the `<vector>` header is already imported in "ioh.hpp".

```C++
#include "ioh.hpp"

std::vector<double> new_problem(const std::vector<double> &)
{
    // the actual function body start here
    // ...
}
```

Then, you only need to "wrap" this new function as follows:

```c++
ioh::problem::wrap_function<double>(&new_problem,
                                    "new_problem" // name for the new function
                                    );
```

In addition, you can define transformation methods for generating various instances of the function.

```c++
std::vector<double> new_transform_variables_function(const std::vector<double> &x, int instance_id) {
  // The instance_id can be used as seed for possible randomizing process.
  // ...
}

double new_transform_objective_functions(double y, int instance_id) {
  // The instance_id can be used as seed for possible randomizing process.
  // ...
}

ioh::problem::wrap_function<double>(&new_problem,  // the new function
                                    "new_problem", // name of the new function
                                    ioh::common::OptimizationType::Minimization, // optimization type
                                    0,  // lowerbound  
                                    1,  // upperbound
                                    &new_transform_variables_function, // the variable transformation method. Optional argument when transformation is applied.
                                    &new_transform_objective_functions // the objective transformation method. Optional argument when transformation is applied.
                                    );
```

After wrapping, we could create this `new_problem` from the problem factory. Note that,
the instance id is ineffective in this approach since we haven't implemented any transformations for the wrapped problem.

```c++
auto &factory = ioh::problem::ProblemRegistry<ioh::problem::Real>::instance();
auto new_problem_f = factory.create(
  "new_problem",  // create by name
  1,               // instance id
  10               // number of search variables
);
```

Alternatively, one might wish to create the new problem by subclassing the abstract problem class
in IOHexperimenter, taking benefits of implementing more details, e.g., aforementioned transformations.
This can be done by inheriting the corresponding problem registration class, which is

* `ioh::problem::IntegerProblem` for pseudo-Boolean problems, and
* `ioh::problem::RealProblem` for continuous problems.

In the below example, we show how to do this for pseudo-Boolean problems.

```C++
class NewBooleanProblem final : public ioh::problem::IntegerProblem<NewBooleanProblem>
{
protected:
    // [mandatory] The evaluate method is mandatory to implement
    std::vector<int> evaluate(const std::vector<int> &x) override
    {
        // the function body goes here
    }

    // [optional] If one wish to implement transformations on objective values
    std::vector<double> transform_objectives(std::vector<double> y) override
    {

    }

    // [optional] If one wish to implement transformations on search variables
    std::vector<double> transform_objectives(std::vector<double> y) override
    {

    }

public:
    /// [mandatory] This constructor always take `instance` as input even
    /// if it is ineffective by default. `instance` would be effective if and only if
    /// at least one of `transform_objectives` and `transform_objectives` is implemented
    NewBooleanProblem(const int instance, const int n_variables) :
        IntegerProblem(
          ioh::problem::MetaData(
            1,                     // problem id, which will be overwritten when registering this class in all pseudo-Boolean problems
            instance,              // the instance id
            "NewBooleanProblem",   // problem name
            n_variables,           // search dimensionality
            1,                     // number of objectives, only support 1 for now
            ioh::common::OptimizationType::Minimization
            )
          )
    {
    }
};
```

Please check [this example](https://github.com/IOHprofiler/IOHexperimenter/blob/8a49d76d591c52b4ae8ed0991d4b6ea8d5c3adaa/example/problem_example.h#L52) for adding continuous problems in this manner.

### Use a logger

We provide a default logger to track and record function evaluations during the optimization process into csv files. The structure and format of csv files is introduced [here](https://arxiv.org/pdf/1810.05281.pdf).

The default logger can be initialized by

```cpp
inline ioh::logger::Analyzer get_logger(const std::string &folder_name = "logger_example", const bool store_positions = false)
{
    /// Instantiate a logger.
    using namespace ioh;
    return logger::Analyzer(
        {trigger::on_improvement}, // trigger when the objective value improves
        {},                        // no additional properties 
        fs::current_path(),        // path to store data
        folder_name,               // name of the folder in path, which will be newly created
        "PSO",                     // name of the algoritm 
        "Type1",                   // additional info about the algorithm              
        store_positions            // where to store x positions in the data files 
    );
}
```

You can either attach a logger to a suite by calling ```suite->attach_logger(logger)```or a new problem (in a suite manually) by calling ```problem->attach_logger(logger)```.
Note that the logger must be attached to the suite or the problem before any function evaluation happens.
By attaching the logger to a problem, information (best-found fitness so far, etc.) of each function evaluation during the optimization process will be recorded automatically.

In addition, the default logger supports recording the algorithms' parameters.

```cpp
// Add parameters fixed throughout the experiment, which are stored in *.info files.
logger.add_experiment_attribute("meta_data_x", "69");
logger.add_experiment_attribute("meta_data_y", "69");

// Declare parameters unique for each run, which are stored in *.info files.
logger.add_run_attribute("run_id", &run_id);
    
// Add dynamic parameters, which are stored as columns in *.dat files. 
logger.watch(ioh::watch::address("x0", x.data()));
logger.watch(ioh::watch::address("x1", x.data() + 1));
```

### Using Experiment

`Experimenter` class automatically tests the given `solver` on the pre-defined `suite` of problems and record the optimization process using a `logger`. The usage of `suite` and `logger` is introduced above.

```cpp
// You can use ioh::problem::Integer for discrete optimization.
void solver(const std::shared_ptr<ioh::problem::Real> p)
{
  ...
}

const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();
const auto suite = suite_factory.create("BBOB", {1, 2}, {1, 2}, {5, 10});
const auto logger = std::make_shared<ioh::logger::Default>(std::string("logger-experimenter"));

ioh::experiment::Experimenter<ioh::problem::Real> f(suite, logger, solver, 10);
f.run();
```

For the detailed documentation of all available functionality in the __IOHexperimenter__, please check our [this page](https://iohexperimenter.readthedocs.io/en/restru/index.html) __[under construction]__.

<!-- ### Using IOHexperimenter in R
To use the IOHexperimenter within `R`, please visit the [R branch](https://github.com/IOHprofiler/IOHexperimenter/tree/R) of this repository. -->

<!-- ## Python Interface

The python interface is avaible via [pip](https://pypi.org/project/ioh): ```pip install ioh```. Please see [here](https://github.com/IOHprofiler/IOHexperimenter/tree/master/ioh) for the functionality it provides for now and some example use cases. -->
