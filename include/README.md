## Quickstart

### Installation

The following toolkits are needed for compiling IOHexperimenter:

* A `C++` compiler. The minimum compiler version is g++7 or equivalent, but we recommend g++9 or equivalent.
* [CMake](https://cmake.org), version 3.15 or higher

Please use the following commands to download, compile, and install this package:

```sh
> git clone --recursive https://github.com/IOHprofiler/IOHexperimenter.git
> cd IOHexperimenter
> mkdir build
> cd build
> cmake .. && make install
```

which installs the header files to `/usr/local/include/ioh` and the compiled `ioh` library, which holds the
registries of the built-in problems and suites and the BBOB problem classes, to `/usr/local/lib` by default.
If you want to change this directory, please use the following flag `cmake -DCMAKE_INSTALL_PREFIX=your/path ..`

The library is built optimized (`Release`) unless you choose another `CMAKE_BUILD_TYPE`. With a
multi-configuration generator such as Visual Studio, build and install a configuration explicitly, e.g.
`cmake --build . --config Release --target install`; the Debug library is called `iohd`, and on MSVC your own
build must use the matching one.

Every program using IOHexperimenter links against the library, which also adds the include directories:

```cmake
find_package(ioh REQUIRED)   # or add_subdirectory(IOHexperimenter)
target_link_libraries(your_target PRIVATE ioh::ioh)
```

If you want to change build options, check the output of `cmake -L` or use `cmake-gui` or `ccmake`.

### Dependencies
IOHexperimenter has a few dependencies, however, only the fmt/fmt library is required to compile the C++ backend. If you'd like to compile the source code without CMake, you need to provide the includes of fmt/fmt as an additional include directory, which can be found in the [external](https://github.com/IOHprofiler/IOHexperimenter/tree/master/external) folder of the repository, and compile the sources in `src/` along with your own: they hold the registries of the built-in problems and suites and the BBOB problem classes, so no program links without them.

### Usage

To obtain a built-in problem, you could create a problem instance by passing the
*instance id* and the *search dimension* to the constructor of the intended problem class, e.g.,

```C++
#include "ioh.hpp"
auto om = ioh::problem::pbo::OneMax(1, 10); // PBO problem: instance 1, dim 10
auto sp = ioh::problem::bbob::Sphere(1, 5); // BBOB problem: instance 1, dim 5
```

The instance id is intended to generalize a certain problem by some transformations, where
it serves as the random seed for randomizing the transformations, e.g., affine
transforms for BBOB problems and scaling of objective values for PBO problems. Please see

* [PBO transformations](https://iohprofiler.github.io/IOHproblem/)
* [BBOB/COCO transformations](https://coco.gforge.inria.fr/downloads/download16.00/bbobdocfunctions.pdf)

Alternatively, we can also obtain a `std::shared_ptr` to a problem by using a problem registry of a given type. Using the registry, we can obtain problems by their name, which can be usefull in a dynamic setting:

```C++
const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::IntegerSingleObjective>::instance();
const std::shared_ptr<ioh::problem::IntegerSingleObjective> om = problem_factory.create("OneMax", 1, 10);
```

`Problem` objects are callable objects, and a search point can be evaluated on a problem by passing it to the call operator of the problem, for example:

```C++
std::vector<double> x0 = {1., 2., 3., 4., 5.};
double y0 = sp(x0);
```

For more advanced usage examples, please take a look at the example folder in the github repository. Some examples include:

* Using a [problem](./example/problem_example.hpp)
* Using a pre-defined [problem suite](./example/suite_example.hpp)
* Using the [logger](./example/logger_example.hpp) for storing benchmark data

### Adding new problems

We offer a very simple and convenient interface for integrating new benchmark problems/functions. First, you could define a new `test_problem` as you like. Note that the `<vector>` header is already imported in "ioh.hpp".

```C++
ioh::problem::SingleObjective test_problem(const std::vector<double> &)
{
    // the actual function body starts here
    // ...
    return 0.0;
}
```

Then, you only need to "wrap" this new function as follows:

```c++
ioh::problem::wrap_function<double, ioh::problem::SingleObjective>(
  &test_problem,
  "test_problem" // name for the new function
);
```

After wrapping, we could create this `test_problem` from the problem factory. Note that,
the instance id is ineffective in this approach since we haven't implemented any transformations for the wrapped problem.

```c++
auto &factory = ioh::problem::ProblemRegistry<ioh::problem::RealSingleObjective>::instance();
auto new_problem_f = factory.create(
  "test_problem",  // create by name
  1,               // instance id
  10               // number of search variables
);
```

Alternatively, one might wish to create the new problem by subclassing the abstract problem class
in IOHexperimenter, taking benefits of implementing more details, e.g., aforementioned transformations.
This can be done by inheriting the corresponding problem class, which is

* `ioh::problem::IntegerSingleObjective` for pseudo-Boolean problems, and
* `ioh::problem::RealSingleObjective` for continuous problems.

In the below example, we show how to do this for pseudo-Boolean problems.

```C++
class NewBooleanProblem final : public ioh::problem::IntegerSingleObjective
{
protected:
    // [mandatory] The evaluate method is mandatory to implement
    double evaluate(const std::vector<int> &x) override
    {
        // the function body goes here
        return 0.0;
    }

    // [optional] If one wish to implement transformations on search variables
    std::vector<int> transform_variables(std::vector<int> x) override
    {
        return x;
    }

    // [optional] If one wish to implement transformations on objective values
    double transform_objectives(const double y) override
    { 
        return y;
    } 

public:
    /// [mandatory for registering] The id and name in the registry, also used by the constructor. If the id is
    /// taken when registering, the problem gets the next free one.
    static constexpr int registry_id = 9001;
    static constexpr const char *registry_name = "NewBooleanProblem";

    /// [mandatory] This constructor always take `instance` as input even
    /// if it is ineffective by default. `instance` would be effective if and only if
    /// at least one of `transform_objectives` and `transform_objectives` is implemented
    NewBooleanProblem(const int instance, const int n_variables) :
        ioh::problem::IntegerSingleObjective(
          ioh::problem::MetaData(
            registry_id,           // the problem id
            instance,              // the instance id
            registry_name,         // the problem name
            n_variables           // search dimensionality
            ) 
          )
    {
    }
};
```

To create it by id or name from the factory of all pseudo-Boolean problems, like the built-in problems, register it
once before use:

```C++
ioh::problem::register_problem<NewBooleanProblem, ioh::problem::IntegerSingleObjective>();
```

A class whose data defines several problems lists them instead, in a static `registry_entries()` that returns one
`ioh::problem::RegistryEntry` (id, name, and a function creating the problem for an instance and a dimension) per
problem, as the CEC2013 and graph problems do. `register_problem<T, Parent>(id, name)` registers a class under an id
and name of your choice.

Please check [this example](./example/problem_example.hpp) for adding continuous problems in this manner.
