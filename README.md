# IOHprofiler: IOHexperimenter

The __experiment tool__ for <b>I</b>terative <b>O</b>ptimization <b>H</b>euristics (IOHs).

* __Documentation__: [https://arxiv.org/abs/1810.05281](https://arxiv.org/abs/1810.05281)
* __Wiki page__: [https://iohprofiler.github.io/IOHanalyzer](https://iohprofiler.github.io/IOHanalyzer)
* __General Contact__: [iohprofiler@liacs.leidenuniv.nl](iohprofiler@liacs.leidenuniv.nl)
* __Mailing List__: [https://lists.leidenuniv.nl/mailman/listinfo/iohprofiler](https://lists.leidenuniv.nl/mailman/listinfo/iohprofiler)

It <i>provides</i> a tool for:

* testing algorithm and generating result data with format of __IOHanalyzer__
* creating new test problems
* configuring customized test suites (collection of problems)
* mutliple data logging options (__in development__)
* framework of algorithm design (__in development__)

It is <i>built on</i>:

* `C++` (tested on `gcc 5.4.0`)
* `boost.filesystem` library for logging files.

It supports:

* `C++`
* `R` package [https://github.com/IOHprofiler/IOHexperimenter/tree/R](https://github.com/IOHprofiler/IOHexperimenter/tree/R)

## Using IOHexperimenter

### Running Experiments

The __IOHexperimenter__ has been built on `C++` and tested on complier `gcc 5.4.0`. To use the class logging `csv` output files, `boost.filesystem` library is required (To install boost library, please visit [https://www.boost.org](https://www.boost.org)).

#### Using by C++

If you are using the tool for the first time, please download or clone the project and run `make` at the root directory of the project. After running `make` to compile,
* object files will be generated in `build/c/obj`
* three exectuable files will be generated in `build/c/bin`

Afterwards, you can copy the folder `build/c` and use the `Makefile` for your future work.
For the details of `C++` experiments, please visit [here](/build/C).

#### Using by R
For the use of `R`, please visit [R branch](https://github.com/IOHprofiler/IOHexperimenter/tree/R).

### Creating test problems

The supported problems of __IOHexperimenter__ are with a vector of variables with a unique type and a vector of real objectives. For detailed steps of creating a problem, please visit [here](/src/Problems).

### Configuring test suites
Suites are collections of test problems. By including problems into a suite, it is easier for users to maintain their experiments.
Note that the variables type of problems of a suite need to be the same. For detailed steps of creating and using suites, please visit [here](/src/Suites).

## Contact

If you have any questions, comments, suggestions or pull requests, please don't hesitate contacting us <IOHprofiler@liacs.leidenuniv.nl>!

## Our team

* [Furong Ye](https://www.universiteitleiden.nl/en/staffmembers/furong-ye#tab-1), <i>Leiden Institute of Advanced Computer Science</i>,
* Diederick Vermetten, <i>Leiden Institute of Advanced Computer Science</i>,
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
