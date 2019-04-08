IOHprofiler: A Benchmarking and Profiling Tool for Iterative Optimization Heuristics
============================================

IOHprofiler is a new tool for analyzing and comparing iterative optimization heuristics.
Given as input algorithms and problems written in C, C++, or Python*, it provides as output an in-depth statistical evaluation of the algorithms’ fixed-target and fixed-budget running time distributions. In addition to these performance evaluations, IOHprofiler also allows to track the evolution of algorithm parameters, making our tool particularly useful for the analysis, comparison, and design of (self-)adaptive algorithms.

IOHprofiler is a ready-to-use software. It consists of two parts: IOHExperimenter, which generates the running time data; and IOHAnalyzer, which produces the summarizing comparisons and statistical evaluations. Currently IOHExperimenter is built on the [COCO](https://github.com/numbbo/coco) software, but a new C++ based version is developing and will be released soon.

[This code] implements the experimentation tool of IOHprofiler. 
For the analyzer part, please visit [IOHAnalyzer page](https://github.com/IOHprofiler/IOHAnalyzer).



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

2. Run the benchmarks using the function 'benchmark_algorithm'
  
3. Using the analyzer by visiting [IOHAnalyzer page](https://github.com/IOHprofiler/IOHAnalyzer)


Using C or Python <a name="Using-C"></a>
---------------
To use the IOHexperimenter in C or python, please look at the other branches in [this repository](https://github.com/IOHprofiler/IOHexperimenter)
