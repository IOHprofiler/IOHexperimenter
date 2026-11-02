# Changelog

## 0.4.0

The first release since 0.3.22. Parts of the Python and C++ APIs changed: the
[upgrade guide](https://iohprofiler.github.io/IOHexperimenter/upgrading.html) says what code written for 0.3 has to
change. The ids, names and values of the built-in problems are unchanged, apart from the fixes below.

### Installation

* Python 3.10 or later (3.9 is no longer supported); 3.12 or later is recommended.
* Wheels for CPython 3.10–3.15 on Linux x86_64 and (new) aarch64, Windows x64 and (new) ARM64 (from 3.11), and macOS
  Intel (10.15 or later) and arm64 (11.0 or later). CPython 3.12 and later share one wheel per platform (`cp312-abi3`),
  which also installs on later versions.
* New free-threaded wheels for CPython 3.14t and 3.15t: the GIL stays disabled.
* The Linux wheels need glibc 2.27 or later on x86_64 (0.3.22: 2.17) and 2.26 or later on aarch64. There are no more
  wheels for 32-bit Linux and Windows. There, and on x86_64 Linux with an older glibc, pip builds the package from
  source, which needs a C++17 compiler; `pip install "ioh<0.4"` installs the 0.3.22 wheels instead.
* The wheels no longer install top-level `tests` and `doc` packages.

### Faster

* The bindings moved from pybind11 to nanobind. Problems take numpy arrays at once instead of element by element: a
  100-dimensional problem evaluates about 8 times faster from a numpy array, and about 2 times (Python 3.10 and 3.11) or
  1.2 times (3.12 and later) faster from a list. Attribute access, state, meta data and suite iteration are faster too.
* ManyAffine (MA-BBOB) problems skip the components with weight 0, whose `sub_problems` are no longer evaluated.
* CEC2022 problems are created about 5 times faster: their data are read once.

### Changed

* Numeric vectors are returned as numpy arrays instead of lists: `LogInfo.x`, `violations` and `penalties`, the batch
  call `problem(X)`, the ManyAffine `weights`, `instances`, `scale_factors` and `function_values`, the DynamicBinVal
  `weights`, `DynamicBinValRanking.rank`, `rank_indices` and `get_comparison_ordering`, `EAH.at` and the matrices in
  `EAH.data`, and the graph weights (read-only views). Overrides of `evaluate`, `transform_variables` and
  `AbstractWModel.wmodel_evaluate` receive `x` as a numpy array instead of a list, like the functions given to
  `wrap_problem`. `Solution.x`, the bounds and the results of `transform_variables` and `calculate_objective` take
  numpy arrays at once.
* The enums (`OptimizationType`, `ConstraintEnforcement`, `StarDiscrepancySampler`) are `enum.Enum` classes; `.name`,
  `.value`, `int(...)` and construction from an int work as before.
* Paths are returned as `pathlib.Path` (`ioh.logger.Path` is `pathlib.Path`), and path arguments take str and
  `os.PathLike`. On Windows, paths with characters outside the ANSI code page work.
* Arguments are converted more strictly: a bool argument takes only `True` and `False` (not `np.bool_`, 0/1 or None),
  an int argument only objects with `__index__`. Integer problems, solutions and bounds take numpy arrays of any integer
  or bool type.
* A problem's id belongs to its family (BBOB, PBO, ...). Registering a problem whose id is taken in its family raises
  `ValueError`, and so does registering under the name of a built-in problem (`wrap_problem(f, "Sphere")` replaced
  Sphere for `get_problem(1)`). Problem classes defined in Python by subclassing report the id they are registered
  under instead of 0, also in logged data.
* The `create` method of the problem classes (e.g. `problem.RealSingleObjective.create`) raises `IndexError` for an
  unknown id or name (in 0.3.22 this was undefined behaviour; `get_problem` raises `ValueError`, as before).
* `load_instances(path)` adds the graphs of a definitions file after the existing problems (from id 2309 on, unless
  other graphs were added before), never replaces built-in graphs, and returns the new ids and names;
  `load_instances()` without a path is deprecated and does nothing.
* Data files: the package uses the data files installed with it, from any working directory, and no longer sets
  `IOH_RESOURCES` (which still names another data directory). `ioh.set_data_root(path)` and `ioh.data_root()` set and
  show the directory. Missing or stale data raise `ioh.DataError` (a `RuntimeError`) when a problem that needs them is
  created, instead of aborting `import ioh`. `download_static_folder()` and `load_graph_problems()` are deprecated and
  do nothing.
* The overrides of Python subclasses of `Store`, `FlatFile` and `Analyzer` (e.g. of `call` or `attach_problem`) are
  now called by the problems and suites they log.
* `LogInfo.objective` and the elements of `CEC2013.optima` are copies: writing to them no longer changes the problem.
* Threads: different problems, loggers and suites can be used from different threads at the same time, also on a
  free-threaded Python; one object is used by one thread at a time. Loggers that run at the same time need their own
  `OnDeltaImprovement` and `OnViolation` triggers instead of the shared `ON_DELTA_IMPROVEMENT` and `ON_VIOLATION`.
* New: `MetaData.fixed_dimension` and `fixed_instance`, whether a problem has one dimension and one instance whatever
  was requested (CEC2013 and graph problems); `MetaData(...)` takes them too.
* Signatures and docstrings follow nanobind's notation; the type stubs include `logger.old` and `problem.sbox`.
* The objective-function server (`ioh-server`) moved to its own repository,
  [IOHprofiler/ioh-objective-server](https://github.com/IOHprofiler/ioh-objective-server).
* The documentation is built from the sources by CI and published at https://iohprofiler.github.io/IOHexperimenter.

### Fixed

* On Windows, the graph problems' ids and names were one too high (`get_problem(2308)` was named
  `PackWhileTravel2309`), also in logged data.
* After a call of the Sobol sampler with a dimension outside 1–1111 raised an error, later points in the previous
  dimension were about 0 (`SobolStarDiscrepancy*` problems, `StarDiscrepancySampler.SOBOL`, C++ `sampler::Sobol`).
* Sobol star discrepancy problems get the same grid whatever problems were created before them.
* Analyzers made with the default triggers shared one best value, so an Analyzer could skip lines because of another's
  run. Analyzers created at the same time, also in different processes, get different folders.
* `EAH.size` and `repr` of an `EAH` without data crashed.
* `Experiment.add_custom_problem` failed when the problem ids were a numpy array (e.g. a suite's `problem_ids`).
* `logger.property.Property(container, attribute)` raised `SystemError` for an attribute that is not a number; it
  raises `TypeError`.

### C++

* The library is no longer header-only. Every program links the `ioh::ioh` target (with CMake) or compiles `src/`: it
  holds the registries of the built-in problems and suites, which are filled when a factory is first used instead of at
  program start, and the BBOB/SBOX problem classes. The default build type is Release; the Debug library is `iohd`.
* Registration: a problem class declares `registry_id` and `registry_name`, or lists its problems in a static
  `registry_entries()`, and is registered with `register_problem<T, Parent>()`, once for each factory; suites declare
  `registry_id` and `registry_name` too and are registered with `register_suite<T>()`. Removed: the registration at
  program start (`RegisteredProblem`, `AutomaticProblemRegistration`, `AutomaticSuiteRegistration`,
  `AutomaticTypeRegistration`, `RegistrationInvoker`, `InvokeApplyOnConstruction`), `InstanceBasedProblem` and its
  `load_instances`, the base classes `BBOProblem`, `PBOProblem`, `CEC2013Problem`, `CEC2022Problem`,
  `DynamicBinValProblem`, `RealSuite` and `IntegerSuite`, `RegisterWithFactory::include`, `IdGetter`, `FactoryID`,
  `type_name` and `class_name`. Suite ids are fixed (Real 1, BBOB 2, SBOX 3, RealStarDiscrepancy 4; Integer 1, PBO 2,
  Submodular 3, IntegerStarDiscrepancy 4).
* `Factory::create` with an unknown id or name throws `std::out_of_range`; registering a problem whose id is taken in
  its family's factory throws `std::invalid_argument`. `Factory::include_with_id(name, first, make)` chooses an id and
  registers under one lock.
* Data: `ioh::data` (`set_root`, `root`, `file`, `data_error`) replaces `get_static_root` and `find_static_file`. The
  library finds its data in the directory given to `ioh::data::set_root`, in `IOH_RESOURCES`, in the source tree it was
  built from or in `share/ioh` of the installation (installed by `cmake --install`); it no longer searches the working
  directory's parents. The test reference files moved from `static/` to `tests/data/`.
* CMake 3.16 or later (also CMake 4). The tests and the example are built by default only when IOHexperimenter is the
  top-level project; the tests are one executable, `test_ioh`, and `ctest` runs from the top of the build directory.
* The server, the CMake option `BUILD_SERVER`, the json and cxxopts submodules and
  `ioh::common::file::merge_output_to_single_folder` are gone.
* `ioh::common::random::GENERATOR` is one generator per thread. An Analyzer given `trigger::on_delta_improvement` (the
  default) has its own `OnDeltaImprovement`.

### Building from source

* The package builds with scikit-build-core instead of `setup.py` (`pip install .`). A rebuild
  (`pip install --no-build-isolation -e .`) compiles only what changed, and a clean build takes about a quarter of the
  time it took for 0.3.22: the problem library is compiled once instead of in every source file of the bindings, and
  the sources are compiled in batches. `ARCHFLAGS` is respected on macOS. `MAKE_STUBS` and `MAKE_DOCS` are replaced by
  `python tools/gen_stubs.py` and `python doc/generate_docs.py`.
* The sdist holds only what building the package needs, with the license of the bundled fmt. The package metadata
  give the license as `BSD-3-Clause`.

## 0.3.22 and earlier

See the [GitHub releases](https://github.com/IOHprofiler/IOHexperimenter/releases).
