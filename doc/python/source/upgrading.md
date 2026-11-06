# Upgrading to 0.4

Version 0.4.0 changes parts of the Python and C++ APIs. This page says what code written for 0.3 has to change; the
[changelog](changelog.md) lists everything that changed. Problem ids, names and values are the same as in 0.3.22.

## Python

### Installation

Python 3.10 or later is needed, and 3.12 or later is recommended: one wheel covers 3.12 and every later version. Where
no wheel fits (32-bit systems, x86_64 Linux with a glibc older than 2.27), pip builds the package from source, which
needs a C++17 compiler. `pip install "ioh<0.4"` installs 0.3.22 instead.

### Numpy arrays instead of lists

Numeric vectors are returned as numpy arrays: `LogInfo.x`, `violations` and `penalties`, the batch call `problem(X)`,
the ManyAffine `weights`, `instances`, `scale_factors` and `function_values`, the DynamicBinVal `weights`,
`DynamicBinValRanking.rank` (a 2-D array), `rank_indices` and `get_comparison_ordering`, `EAH.at` and the matrices in
`EAH.data`, and the graph weights. Python callbacks receive `x` as an array: overrides of `evaluate`,
`transform_variables` and `wmodel_evaluate`, and the `transform_variables` function given to `wrap_problem`.

Code written for lists may need changes:

* `+` adds instead of concatenating: `info.x + [y]` broadcasts without an error. Use `np.append(info.x, y)` or
  `[*info.x, y]`.
* `==` compares element by element, so `if a == b:` and `assertEqual(a, b)` raise; compare `a.tolist()` or use
  `np.array_equal(a, b)`.
* `x * n` multiplies, and the list methods (`append`, `count`, `index`) are gone: `ys.index(min(ys))` becomes
  `np.argmin(ys)`.
* `json` needs `.tolist()`, `isinstance(x, list)` is False, and `str()` of an array has no commas (e.g. in custom file
  writers).
* A returned array is a copy, so writing into it does not change the problem: `problem.bounds.lb[0] = 1` and
  `info.x[0] = 0` have no effect. Assign the attribute instead (`problem.bounds.lb = lb`). The graph weights are
  read-only.
* `Solution.x` stays a writable view of the solution's values. After assigning `x` a vector of another length, read
  `x` again: an array read before may refer to freed memory.
* `LogInfo.objective` and the elements of `CEC2013.optima` are copies: writing to them no longer changes the problem.

### Enums, paths and arguments

* The enums (`OptimizationType`, `ConstraintEnforcement`, `StarDiscrepancySampler`) are `enum.Enum` classes: compare
  members, not ints (`OptimizationType.MIN == 0` was and stays False). `.name`, `.value`, `int(...)` and construction
  from an int work as before.
* Paths are returned as `pathlib.Path` (`ioh.logger.Path` is `pathlib.Path`), and path arguments take str and
  `os.PathLike`.
* A bool argument takes only `True` and `False`, and an int argument only objects with `__index__`: pass `bool(x)` or
  `int(x)` for others (e.g. `np.bool_`, 0 and 1, `Fraction`).
* A subclass of a bound class that does not call `super().__init__()` fails when it is first used.

### Data files

The package uses the data files installed with it, from any working directory. `import ioh` no longer sets
`IOH_RESOURCES`, so subprocesses do not inherit it; the variable still names another data directory.
`ioh.set_data_root(path)` and `ioh.data_root()` change and show the directory for this process. Missing data raise
`ioh.DataError` (a `RuntimeError`) when a problem that needs them is created. `download_static_folder()` and
`load_graph_problems()` are deprecated and do nothing: remove the calls.

### Problem ids and names

* A problem named like a built-in one raises `ValueError`: `wrap_problem(f, "Sphere")` used to replace Sphere for
  `get_problem(1)`. Choose another name.
* A problem whose id is taken in its family (`BBOB`, `PBO`, ...) raises `ValueError`. In the factories of all real and
  all integer problems (`RealSingleObjective`, `IntegerSingleObjective`) it gets the next free id as its key and keeps
  its own id.
* Problem classes defined by subclassing report the id they are registered under instead of 0, also in logged data.
* `load_instances(path)` adds the graphs of a file after the existing problems and returns their ids and names: use
  those ids. `load_instances()` without a path is deprecated and does nothing.
* `create` with an unknown id or name raises `IndexError`.

### Loggers and threads

* The overrides in Python subclasses of `Store`, `FlatFile` and `Analyzer` are now called by the problems and suites
  they log; before, they were ignored.
* Use one object from one thread at a time. Loggers that run at the same time need their own `OnDeltaImprovement` and
  `OnViolation` triggers instead of the shared `ON_DELTA_IMPROVEMENT` and `ON_VIOLATION`, and the problems made by one
  `wrap_problem` call with constraints are evaluated from one thread at a time (they share the constraint objects).

### The old Analyzer

The Analyzer that wrote `.info` files is gone. `ioh.logger.old` (and `ioh.logger.old.Analyzer`) and the
`old_logger` argument of `ioh.Experiment` were removed: `ioh.logger.Analyzer` and `ioh.Experiment` write the JSON
metadata plus `.dat` files, as before. IOHanalyzer reads this format, and iohinspector still reads existing `.info`
data, but nothing writes it any more. Drop `old_logger=True` and `ioh.logger.old`.

### Building from source

The package builds with scikit-build-core: `pip install .` (or `pip install -e .`); `setup.py` is gone. `MAKE_STUBS`
and `MAKE_DOCS` are replaced by `python tools/gen_stubs.py` and `python doc/generate_docs.py`.

## C++

### Linking

The library is no longer header-only. Every program links it, also one that only constructs problems: the BBOB
classes are compiled in it.

```cmake
find_package(ioh REQUIRED)   # or add_subdirectory(IOHexperimenter)
target_link_libraries(your_target PRIVATE ioh::ioh)
```

Without CMake, compile the sources in `src/` with your program. The default build type is Release, and the Debug
library is called `iohd`. CMake 3.16 or later is needed. A project that adds IOHexperimenter with `add_subdirectory`
no longer builds its tests and example unless it sets `BUILD_TESTS` or `BUILD_EXAMPLE`. The tests are one executable,
`test_ioh`, and `ctest` runs from the top of the build directory; `-DIOH_TEST_PER_FILE=ON` also builds one executable
per test file.

### Registering problems

The problems are no longer registered at program start. A problem class declares `registry_id` and `registry_name`
(or lists its problems in a static `registry_entries()`) and is registered with `register_problem<T, Parent>()`. One
call fills one factory, so call it for each factory the old base class filled:

* `BBOProblem`: `BBOB` (with `P<BBOB>`), `SBOX` (with `P<SBOX>`) and `RealSingleObjective` (with `P<BBOB>`);
* `PBOProblem`: `PBO` and `IntegerSingleObjective`;
* `CEC2013Problem`, `CEC2022Problem`: `CEC2013` or `CEC2022`, and `RealSingleObjective`;
* `DynamicBinValProblem`: `DynamicBinVal` and `IntegerSingleObjective`.

Derive from `BBOB`/`SBOX`, `PBO`, `CEC2013`, `CEC2022` or `DynamicBinVal` directly. The id belongs to the problem's
family: in a family's factory (`BBOB`, `PBO`, ...) a taken id throws `std::invalid_argument`, so choose one that is
free there; in `RealSingleObjective` and `IntegerSingleObjective` a taken id is replaced by the next free one as the
registry key, while the problem keeps its own id. Registering under the name of a built-in problem throws.
`register_problem<T, Parent>(id, name)` registers a class under an id and name of your choice.

A `GraphProblemType` subclass's specialization of `InstanceBasedProblem::load_instances` no longer compiles. Give the
class

```cpp
static std::vector<RegistryEntry<T>> registry_entries(const std::optional<fs::path> &path = std::nullopt);
```

returning `graph_entries(path or its default file, "Name")` (formerly `get_constructors(path)`; pass the class name as
`"Name"` to keep the old registry names, the class name and the id), keep its `default_id`, and register it with
`register_problem<T, GraphProblem>()` and `register_problem<T, IntegerSingleObjective>()`, or with
`GraphProblemType<T>::load_graph_instances<int, int>()`. Both register the class's own graphs with ids from
`default_id` on (a taken id throws). `load_graph_instances(path)` adds the graphs of another definitions file after the
existing ids and returns them.

### Registering suites

A suite declares `registry_id` and `registry_name` and is registered with `register_suite<T>()`; derive from
`Suite<...>` instead of `RealSuite` or `IntegerSuite` (which registered a suite under its class name with the next free
id). The built-in suites have fixed ids: Real 1, BBOB 2, SBOX 3, RealStarDiscrepancy 4; Integer 1, PBO 2, Submodular 3,
IntegerStarDiscrepancy 4.

### The old Analyzer

`ioh::logger::analyzer::v1::Analyzer` (the `.info` writer) is gone, and `ioh::logger::analyzer::v2::Analyzer` is now
`ioh::logger::analyzer::Analyzer`. `ioh::logger::Analyzer` is unchanged and writes the same files. Use it instead of
the `v1` and `v2` names. The old constructor's `use_old_data_format` argument is gone with it.

### Removed and renamed

Removed: `RegisteredProblem`, `AutomaticProblemRegistration`, `AutomaticSuiteRegistration`,
`AutomaticTypeRegistration`, `RegistrationInvoker`, `InvokeApplyOnConstruction`, the base classes `BBOProblem`,
`PBOProblem`, `CEC2013Problem`, `CEC2022Problem`, `DynamicBinValProblem`, `RealSuite` and `IntegerSuite`,
`InstanceBasedProblem` and `load_instances`, `RegisterWithFactory::include`, `IdGetter`, `FactoryID`, `type_name`,
`class_name`, `get_static_root`, `find_static_file` and `ioh::common::file::merge_output_to_single_folder`. Renamed:
`GraphProblemType::get_constructors(path)` is `graph_entries(path, name)`, and
`star_discrepancy::load_default_instances` is `default_registry_entries`. `Factory::create` with an unknown id or name
throws `std::out_of_range` (it was undefined behaviour in release builds).

### Data files

The library finds its data with `ioh::data::root()`: the directory given to `ioh::data::set_root(path)`, else
`IOH_RESOURCES`, else the source tree it was built from, else `share/ioh` of the installation (installed by
`cmake --install`). It no longer searches the working directory's parents for `IOHexperimenter/static`. Missing data
throw `ioh::data::data_error` instead of returning an empty path or aborting, and the data directory must match the
library's data version (its `manifest.json`). Use `ioh::data::root()` and `ioh::data::file()` instead of
`get_static_root` and `find_static_file`. A graph class's files are resolved when it is loaded; `graph::Meta::root` is
empty for the built-in graphs.

### Server

The objective-function server moved to
[IOHprofiler/ioh-objective-server](https://github.com/IOHprofiler/ioh-objective-server), which builds against the
library. `-DBUILD_SERVER`, `HAS_JSON` and the `external/json` and `external/cxxopts` submodules are gone.

### Threads

`ioh::common::random::GENERATOR` is one generator per thread, so `seed` seeds the calling thread's generator.
`Factory::include_with_id(name, first, make)` chooses an id and registers under one lock. An Analyzer given
`trigger::on_delta_improvement` (the default) has its own `OnDeltaImprovement` with the same delta.
