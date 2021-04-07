# IOHprofiler: IOHexperimenter
![Ubuntu g++-{10, 9, 8}](https://github.com/IOHprofiler/IOHexperimenter/workflows/Ubuntu/badge.svg)
![MacOS clang++, g++-{9, 8}](https://github.com/IOHprofiler/IOHexperimenter/workflows/MacOS/badge.svg)
![Windows MVSC-2019](https://github.com/IOHprofiler/IOHexperimenter/workflows/Windows/badge.svg)


__Experimenter__ for <b>I</b>terative <b>O</b>ptimization <b>H</b>euristics (IOHs), built natively in</i> `C++`.

* __Documentation__: [https://arxiv.org/abs/1810.05281](https://arxiv.org/abs/1810.05281)
* __Wiki page__: [https://iohprofiler.github.io](https://iohprofiler.github.io/)
* __General Contact__: [iohprofiler@liacs.leidenuniv.nl](iohprofiler@liacs.leidenuniv.nl)
<!-- * __Mailing List__: [https://lists.leidenuniv.nl/mailman/listinfo/iohprofiler](https://lists.leidenuniv.nl/mailman/listinfo/iohprofiler) -->

<b>IOHexperimenter</b> <i>provides</i>:

* A framework to ease the benchmarking of any iterative optimization heuristic
* Continuous and discrete benchmarking problems
* [Pseudo-Boolean Optimization (PBO)](https://iohprofiler.github.io/IOHproblem/) problem set (25 pseudo-Boolean problems)
* Integration of the well-known [Black-black Optimization Benchmarking (BBOB)](https://github.com/numbbo/coco) problem set (24 continuous problems)
* Interface for adding new problems and suite/problem set
* Advanced logging module that takes care of registering the data in a seamless manner
* Data format is compatible with [IOHanalyzer](https://github.com/IOHprofiler/IOHanalyzer)

<b>IOHexperimenter</b> is available for:

* `C++` manual can be found [here](https://iohprofiler.github.io/IOHexp/Cpp/)
* `Python`: please see [here](https://github.com/IOHprofiler/IOHexperimenter/tree/master/ioh) for details user manual
<!-- * or as a [pip package](https://pypi.org/project/IOHexperimenter/); [Wiki Page](https://iohprofiler.github.io/IOHexp/python/). -->

## C++ Interface

### Installation

The following toolkits are needed for compiling IOHexperimenter:

* A `C++` compiler. The minimum compiler version is g++ 7 or equivalent, but we recommend g++ 9 or equivalent.
* [CMake](https://cmake.org), version 3.10 or higher

Please use the following commands to download, compile, and install this package:

```sh
> git clone https://github.com/IOHprofiler/IOHexperimenter.git
> cd IOHexperimenter
> mkdir build
> cd build
> cmake .. && make install
```

which installs all header files to `/usr/local/include/ioh` by default. If you want to change this directory, please use the following flag `cmake -DCMAKE_INSTALL_PREFIX=your/path .`

### Examples

We provide some simple examples to demonstrate the basic usage:

* Using [a single problem](https://github.com/IOHprofiler/IOHexperimenter/blob/master/example/problem_example.h)
* Using a pre-defined [problem suite/set](https://github.com/IOHprofiler/IOHexperimenter/blob/master/example/suite_example.h)
* Using the [logging ability](https://github.com/IOHprofiler/IOHexperimenter/blob/master/example/logger_example.h) for storing benchmark data

For the detailed documentation of all available functionality in the __IOHexperimenter__, please check our [this page](https://iohexperimenter.readthedocs.io/en/restru/index.html) __[under construction]__.

### Add new problems

We offer a very simple and convenient interface for integrating new benchmark problems/functions. First, you could define a new `test_problem` as you like. Note that the `<vector>` header is already imported in "ioh.hpp".

```C++
#include "ioh.hpp"

std::vector<double> test_problem(const std::vector<double> &)
{
    // the actual function body start here
    // ...
}
```

Then, you only need to "wrap" this new function as follows:

```c++
auto new_problem = ioh::problem::wrap_function<double>(
  &test_problem,
  "test_problem" // name for the new function
);
std::cout << const_z_problem.meta_data() << std::endl;
```

After wrapping, we could also create this `test_problem` from the problem factory:

```c++
auto &factory = ioh::problem::ProblemRegistry<ioh::problem::Real>::instance();
auto new_problem_f = factory.create("test_problem", 1, 10);
```

Alternatively, one might wish to create the new problem by subclassing the abstract problem class in IOHexperimenter.
This can be done by inheriting the corresponding problem registration class, which is `ioh::problem::IntegerProblem`
for pseudo-Boolean problems and `ioh::problem::RealProblem` for continuous problems. In the below example, we show
how to do this for pseudo-Boolean problems.

```C++
class NewBooleanProblem final : public ioh::problem::IntegerProblem<NewBooleanProblem>
{
protected:
    // The evaluate method is required, in this case the value of x0 is return as objective value
    std::vector<int> evaluate(const std::vector<int> &x) override
    {
        // the function body
    }

public:
    /// This constructor is required(i.e. (int, int), even if the newly create problem does not have a way to handle different
    /// instances/dimensions.
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

Please check [this example](https://github.com/IOHprofiler/IOHexperimenter/blob/759750759331fff1243ef9e121209cde450b9726/example/problem_example.h#L51) for adding continuous problems in this manner.

<!-- ### Using IOHexperimenter in R
To use the IOHexperimenter within `R`, please visit the [R branch](https://github.com/IOHprofiler/IOHexperimenter/tree/R) of this repository. -->

## Python Interface

The Python interface is still under construction at this momement. Please see [here](https://github.com/IOHprofiler/IOHexperimenter/tree/master/ioh) for the functionality it provides for now and some example use cases.

## Contact

If you have any questions, comments or suggestions, please don't hesitate contacting us <IOHprofiler@liacs.leidenuniv.nl>.

## Our team

* [Jacob de Nobel](https://www.universiteitleiden.nl/en/staffmembers/jacob-de-nobel), <i>Leiden Institute of Advanced Computer Science</i>,
* [Furong Ye](https://www.universiteitleiden.nl/en/staffmembers/furong-ye#tab-1), <i>Leiden Institute of Advanced Computer Science</i>,
* [Diederick Vermetten](https://www.universiteitleiden.nl/en/staffmembers/diederick-vermetten#tab-1), <i>Leiden Institute of Advanced Computer Science</i>,
* [Hao Wang](https://www.universiteitleiden.nl/en/staffmembers/hao-wang#tab-1), <i>Leiden Institute of Advanced Computer Science</i>,
* [Carola Doerr](http://www-desir.lip6.fr/~doerr/), <i>CNRS and Sorbonne University</i>,
* [Thomas Bäck](https://www.universiteitleiden.nl/en/staffmembers/thomas-back#tab-1), <i>Leiden Institute of Advanced Computer Science</i>,

When using IOHprofiler and parts thereof, please kindly cite this work as

Carola Doerr, Hao Wang, Furong Ye, Sander van Rijn, Thomas Bäck: <i>IOHprofiler: A Benchmarking and Profiling Tool for Iterative Optimization Heuristics</i>, arXiv e-prints:1810.05281, 2018.

```bibtex
@ARTICLE{IOHprofiler,
  author = {Carola Doerr and Hao Wang and Furong Ye and Sander van Rijn and Thomas B{\"a}ck},
  title = {{IOHprofiler: A Benchmarking and Profiling Tool for Iterative Optimization Heuristics}},
  journal = {arXiv e-prints:1810.05281},
  archivePrefix = "arXiv",
  eprint = {1810.05281},
  year = 2018,
  month = oct,
  keywords = {Computer Science - Neural and Evolutionary Computing},
  url = {https://arxiv.org/abs/1810.05281}
}
```
