# IOHprofiler: IOHexperimenter 
![Ubuntu g++-{10, 9, 8}](https://github.com/IOHprofiler/IOHexperimenter/workflows/Ubuntu/badge.svg)
![MacOS clang++, g++-{9, 8}](https://github.com/IOHprofiler/IOHexperimenter/workflows/MacOS/badge.svg)
![Windows MVSC-2019](https://github.com/IOHprofiler/IOHexperimenter/workflows/Windows/badge.svg)


This is the __benchmarking platform__ for <b>I</b>terative <b>O</b>ptimization <b>H</b>euristics (IOHs).

* __Documentation__: [https://arxiv.org/abs/1810.05281](https://arxiv.org/abs/1810.05281)
* __Wiki page__: [https://iohprofiler.github.io](https://iohprofiler.github.io/)
* __General Contact__: [iohprofiler@liacs.leidenuniv.nl](iohprofiler@liacs.leidenuniv.nl)
* __Mailing List__: [https://lists.leidenuniv.nl/mailman/listinfo/iohprofiler](https://lists.leidenuniv.nl/mailman/listinfo/iohprofiler)

<b>IOHexperimenter</b> <i>provides</i>:

* A framework for straightforward benchmarking of any iterative optimization heuristic
* A suite consisting of 25 pre-made Pseudo-Boolean benchmarking function, with easily accessible methods for adding custom functions and suites 
* Logging methods to effortlesly store benchmarking data in a format compatible with __IOHanalyzer__, with future support for additional data logging options

<b>IOHexperimenter</b> is available for:

* `C++` on the current GitHub branch; [Wiki Page](https://iohprofiler.github.io/IOHexp/Cpp/).
* `R` on [this GitHub branch](https://github.com/IOHprofiler/IOHexperimenter/tree/R) or as a [CRAN package](https://cran.r-project.org/package=IOHexperimenter); [Wiki Page](https://iohprofiler.github.io/IOHexp/R/).
* `Python` on [this GitHub branch](https://github.com/IOHprofiler/IOHexperimenter/tree/python-interface) or as a [pip package](https://pypi.org/project/IOHexperimenter/); [Wiki Page](https://iohprofiler.github.io/IOHexp/python/).

### Using IOHexperimenter in C++

<b>IOHexperimenter</b> is <i>built natively in</i> `C++`. To use this version of the <b>IOHexperimenter</b>, the following dependencies are needed:

* A `C++` compiler. The minimum compiler version is g++ 7 or equivalent, but we recommend g++ 9 or equivalent.
* [Cmake](https://cmake.org), version 3.10 or higher

If you are using the tool for the first time, please download or clone this branch, go to the directory where the project root is located and run the following: 
```
cmake .
make install
```
* If you want to set up the install directory, please run `cmake -DCMAKE_INSTALL_PREFIX=your/path .` before installation.
* three exectuable files will be generated in `build/Cpp` for test.

Note: If you want to set up the install directory, replace the first command with `cmake -DCMAKE_INSTALL_PREFIX=your/path .` where 'your/path' is the required installation directory

After installation, three exectuable files will be generated in the `example` folder. These can be used for testing the IOHexperimenter, and their source provides an easy starting point for running the IOHexperimenter in the three most common ways:
__NOTE: THESE LINKS STILL HAVE TO BE UPDATED TO THE NEW STRUCTURE__
* Running an algorithm on a [single function](https://iohprofiler.github.io/IOHexp/Cpp/#using-individual-problems)
* Running an algorithm on a [suite of functions](https://iohprofiler.github.io/IOHexp/Cpp/#using-suites)
* Using the [IOHexperimenter class to benchmark based on a configuration file](https://iohprofiler.github.io/IOHexp/Cpp/#using-conf)

For more details of how to use the `C++` version, please visit [our wiki on this subject](/build/Cpp).

For the detailed documentation of all available functionality in the __IOHexperimenter__, please visit [this page](https://iohexperimenter.readthedocs.io/en/restru/index.html)

### Using IOHexperimenter in R
To use the IOHexperimenter within `R`, please visit the [R branch](https://github.com/IOHprofiler/IOHexperimenter/tree/R) of this repository.

### Using IOHexperimeter in Python
To use the IOHexperimenter in `Python`, please visit the [python branch](https://github.com/IOHprofiler/IOHexperimenter/tree/python-interface) of this repository.

### Expanding the IOHexperimenter 

Benchmarking problems in __IOHexperimenter__ are easy to create yourself. We provide support for any input type and any number of real-valued objectives. For a more detailed guidline of how to define a benchmarking problem within IOHexperimenter, please visit [this page](/src/Problems).

Suites are collections of benchmarking problems. By including problems into a suite, it is easier for users to maintain their experiments. If you create a set of similar problems, it is recommended to create a suite to collect them together, which can be done effortlesly within the IOHexperimenter. For detailed steps of creating and using suites, please visit [this page](/src/Suites).

## Contact

If you have any questions, comments or suggestions, please don't hesitate contacting us <IOHprofiler@liacs.leidenuniv.nl>!

## Our team

* [Furong Ye](https://www.universiteitleiden.nl/en/staffmembers/furong-ye#tab-1), <i>Leiden Institute of Advanced Computer Science</i>,
* [Jacob de Nobel](https://www.universiteitleiden.nl/en/staffmembers/jacob-de-nobel), <i>Leiden Institute of Advanced Computer Science</i>,
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
