IOHprofiler: A Benchmarking and Profiling Tool for Iterative Optimization Heuristics
============================================

R-package  <a name="package"></a>
------------
This branch of the IOHexperimenter consists of the R-implementation of the experimenter, built using Rcpp.
To use this package, either clone this repository and install locally, or use the following commands to use devtools to install the package directely:

If devtools is not yet installed, please first use
```r
install.packages('devtools')
```
Error messages will be shown in your R console if there is any installation issue.
Now, the IOHexperimenter package can be installed and loaded using the following commands:
```r
devtools::install_github('IOHprofiler/IOHexperimenter@R')
library('IOHexperimenter')
```
This will install the package and all required dependencies.


Getting Started <a name="Getting-Started"></a>
---------------
0. Install the package using one of the methods mentioned [_package_](#package) above.

1. Create you own algorithm, or use the example algorithm provided in the documentation accessed by:
```r
?benchmark_algorithm
```

Note that your algorithm will need to accept exactly one parameter: An IOHproblem object, which contains the following information about the current problem:

* dimension
* function_id
* instance
* fopt (if known)
* xopt (if known)

And the following functions:

* obj_func()
* target_hit()
* set_parameters()

Several example algorithms have been implemented in the `algorithms.R` file. 

2. Run the benchmarks using the function 'benchmark_algorithm'
  
3. Analyze your results using the IOHanalyzer by visiting the [IOHAnalyzer page](https://github.com/IOHprofiler/IOHAnalyzer)


Using C or Python <a name="Using-C"></a>
---------------
To use the IOHexperimenter in C++ or python, please look at the other branches in [this repository](https://github.com/IOHprofiler/IOHexperimenter)
