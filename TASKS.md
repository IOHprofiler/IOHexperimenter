# IOHexperimenter: build-time & infrastructure overhaul — task list

This file is the work plan for reducing build times and modernizing the build, CI, registry, data
handling and Python bindings. It is written so that an agent (or a person) can pick up one task at a
time. Work packages (WPs) are merged in order; tasks inside a WP are usually done in the order
listed. **Do not start a WP before its dependencies are merged.**

Each task lists goal, context, steps, acceptance criteria and pitfalls. When a task changes behaviour
or public API it must also update the docs (WP8 consolidates the upgrade guide).

Status: each WP heading below says whether the WP is open, implemented on a branch (see its PR) or
merged.

---

## 0. Background (why we are doing this)

Measured on Apple M5, clang 21, on master before WP2 (full diagnosis in the WP1 PR discussion; the
WP2 section has the numbers after it):

* Building the Python extension (`setup.py`'s CMake flags, measured as in §2) before WP2:
  **≈103–111 s CPU, 40.6 s wall at -j4** (32 s after WP2; a plain `pip install .` also has pip's
  overhead and took 37 s after WP2). About 2/3 of the generated code is the C++ library being
  recompiled in *each* of the 13 binding TUs: `suite_helpers.cpp` (one trivial function) costs as
  much as `real.cpp`; the only TU without `ioh.hpp` costs 0.5 s.
* Root causes, ranked:
  1. **Automatic CRTP registration** (`include/ioh/common/factory.hpp`): `AutomaticTypeRegistration`
     has a reference member whose default initializer odr-uses
     `RegistrationInvoker<T,F>::registration_invoker` (a `static inline` member of a class template).
     Every TU that includes `ioh.hpp` instantiates all **192 registrations of 112 types**
     (104 problems + 8 suites) and emits constructors, vtables and every virtual function of every
     problem. Disabling it cuts generated code by 67 % and binding-TU CPU by 52 %.
  2. **`i8_sobol`** (`include/ioh/common/sobol.hpp`): an `inline` function with 13,224
     `v[i][j] = c;` statements, compiled in every TU that includes `ioh.hpp` (≈5.8 s and 1.6 GB per
     TU at -O3 without LTO), through the star-discrepancy registrations and, in `real.cpp`, the
     `star_discrepancy_init` binding. **Fixed in WP2.**
  3. **Explicit instantiation definitions in headers**: each of the 24 BBOB headers ends with
     `template class Foo<BBOB>;` (ill-formed NDR in a header, emitted in every TU). These lines are
     also the *only* trigger of the BBOB/SBOX registrations — remove them together with (1).
  4. **pybind11's implicit full LTO** (`pybind11_add_module` adds `-flto` / `/GL` in Release):
     a 14.5 s single-threaded, 2.6 GB link on clang (8.2 s after WP2). On master LTO is
     net-positive (it dedups the 13 copies), so turn it off only *after* (1)–(3), per compiler
     (MSVC `/GL` behaves differently).
* Measured result of fixing (1)–(4) with clang: **18.8 s at -j4** (pybind11), ~11.5 s with nanobind,
  evaluation speed unchanged. PCH does not help (the cost is codegen, not parsing). `-Os` for the
  binding TUs makes evaluation 1.2–2.3× slower (header-inline problem code is also emitted, weakly, in
  the `-Os` TUs and the linker keeps the first copy) — keep `-O3`. nanobind's default `-Os` alone
  costs 1.41× on compute-bound problems; use `NOMINSIZE`.
* CI/release state before WP1: releases broken since Dec 2025 (`pypa/cibuildwheel@v3` is not a
  valid ref; the cleanup job deleted the v0.3.23 release and tag); PyPI's latest is 0.3.22
  (cp39–cp312 only); Linux/macOS push CI used retired images and had not run since Sep 2025.
* Data I/O is ad hoc: `get_static_root()` (`include/ioh/common/file.hpp`) scans the current
  directory's ancestors for `IOHexperimenter/static`; Python sets `IOH_RESOURCES` before importing
  `iohcpp` and raises `ImportError` if that directory does not exist; data is read during static
  initialization, so an *existing but incomplete* data dir aborts `import ioh` with SIGABRT; CEC2022
  re-parses its text files on every construction.
* Bugs found while doing WP1 (see §3): graph problem ids off by one on Windows (fixed in WP1),
  MaxInfluence is stochastic, `Factory::create` had undefined behaviour for unknown keys (fixed in
  WP3: it throws).

## 1. Decisions (binding for all tasks)

* **Python API stays functionally identical** (`get_problem(id|name, iid, dim, problem_class)`,
  problem classes, attributes, `wrap_problem`, loggers, suites). Exceptions: the deliberate
  list → numpy change in WP7, and the deprecations listed in WP4. Guarded by the golden tests (T1.1).
* **Problem ids are frozen** to today's values; names are static. No id may change.
* **Registry**: callable-based (`include(name, id, creator)`), populated lazily; the built-in problems
  no longer use CRTP registration (done in WP3; the machinery stays for user types) and
  `InstanceBasedProblem` goes in WP4 (T4.4). C++ API changes are allowed (small C++ user base) but
  must be documented in an upgrade guide. The public aliases `ProblemRegistry<T>`,
  `ProblemFactoryType<T>` and `SuiteRegistry<T>` stay.
* **Ship a compiled `ioh` CMake target** (static library) holding the registry and data code.
* **Data**: stays inside every wheel (~7 MB compressed), plain text formats, described by a
  generated **manifest**.
* **Python floor 3.10**; **cibuildwheel 4.x**; docs state a recommended Python version (≥ 3.12,
  which gets the abi3 wheel after WP6).
* **Bindings move to nanobind** (with `nb::ndarray` inputs), then **numpy arrays are returned
  instead of lists** (separate WP).
* **Wheel matrix**: CPython 3.10–3.15 on Linux x86_64 and aarch64, Windows x64 and ARM64 (from
  3.11: numpy has no Windows ARM64 wheels for 3.10), macOS Intel (`macos-15-intel`) and arm64; no
  win32, i686 or musllinux wheels. Free-threaded wheels (cp314t, cp315t) are built for Linux and
  macOS (on Windows the pinned pybind11 cannot find the free-threaded `python3XXt.lib`; WP6 adds
  them); until WP6 declares the module thread-safe, importing `ioh` on a free-threaded interpreter
  re-enables the GIL (with a `RuntimeWarning`), so until then these wheels only test the
  free-threaded build in CI (the first released ones, in 0.4.0, come after WP6). Pull requests build
  only the Linux and Windows wheels; the full matrix, including macOS, runs weekly, on manual
  dispatch and for releases, because the organization runs at most 20 jobs at once, only 5 of them
  on macOS, and the Intel ones are slow.
* **Problem metadata**: expose `fixed_dimension` and `fixed_instance` (uniform names) on Python
  problems and registry entries (WP3/WP4): `None` when the problem honours the requested
  dimension/instance, otherwise the value it always uses (graph and CEC2013 problems).
* **`static.tar.gz` stays** in the repository: released versions (≤ 0.3.22) download it from
  `master` in `download_static_folder()`. It must be kept identical to `static/` (WP4 adds a check).
* **No intermediate releases**: nothing is released until all WPs are merged. The next release
  (0.4.0, WP8) ships everything at once, so PyPI stays at 0.3.22 until then. To test wheels in
  between, run the wheels workflow by manual dispatch; it only uploads on `release: published`.
* Not doing: PCH, `-Os` for bindings, a separate data wheel, binary data formats, pybind11 3.x.

## 2. Invariants and conventions

* One branch + PR per WP (or per task for big WPs). Keep PRs reviewable; no drive-by refactors.
* Every PR: CI green on Linux, macOS, Windows; golden tests pass unchanged; docs updated.
* Every PR with user-visible changes (Python API, supported platforms or versions, packaging, C++
  API) adds them to the release-notes draft in WP8, because everything ships in one release (§1).
* Pull requests do not build macOS wheels (§1). A PR that changes the build (CMake, `setup.py`,
  `pyproject.toml`, bindings, the wheels workflow) also runs the full wheel matrix on its branch
  before review: `gh workflow run python-publish.yml --ref <branch>`.
* Never regenerate golden fixtures to make a test pass. Fixtures may only be regenerated with explicit
  maintainer approval and a written justification in the PR.
* Keep `-O3` for everything that contains problem code.
* Building locally: submodules are not initialized in a fresh clone and `external/fmt` uses an SSH
  URL; use `git -c url."https://github.com/".insteadOf="git@github.com:" submodule update --init`.
  Python: `pip install -e . -v` (Python ≥ 3.10), then `python -m unittest -v`. C++ tests:
  `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build --target test_ioh --parallel 4`
  and run `build/tests/test_ioh` with `IOH_RESOURCES=$PWD/static` (until WP4).
* Measuring build time (include before/after numbers in PRs that claim build-time changes):
  clean build directory, Release, Ninja, `-j4`, run twice and report the minimum; use
  `-ftime-trace` only for attribution, never for timing (it adds 9–20 %).
* Measuring runtime: construct problems outside the timed region, min of ≥ 5 repetitions, test both
  numpy and list inputs, report relative to master.

## 3. Reference facts

Python-visible factories on master (all frozen by T1.1):

| `ioh.ProblemClass` | C++ factory | entries | ids |
|---|---|---|---|
| `BBOB` | `Factory<BBOB,int,int>` | 24 | 1–24 |
| `SBOX` | `Factory<SBOX,int,int>` | 24 | 1–24 |
| `PBO` | `Factory<PBO,int,int>` | 25 | 1–25 |
| `CEC2013` | `Factory<CEC2013,int,int>` | 20 | 1101–1120 |
| `CEC2022` | `Factory<CEC2022,int,int>` | 12 | 1001–1012 |
| `STAR_REAL` / `STAR_INTEGER` | star-discrepancy factories | 30 each | 30–59 |
| `GRAPH` | `Factory<GraphProblem,int,int>` | 78 | 2000–2004 (MaxCut), 2100–2139 (MaxCoverage), 2200–2223 (MaxInfluence), 2300–2308 (PackWhileTravel) |
| `REAL` | `Factory<RealSingleObjective,int,int>` | 86 | union of BBOB + CEC2013 + CEC2022 + STAR_REAL |
| `INTEGER` | `Factory<IntegerSingleObjective,int,int>` | 137 | union of PBO + GRAPH + STAR_INTEGER + DynamicBinVal |
| (no enum) `problem.DynamicBinVal` | `Factory<DynamicBinVal,int,int>` | 4 | 10001–10004 |

Every bound problem class also exposes a `problems` property returning one of these maps (96
Python-visible views, 3,256 entries).

**Where ids and names come from today**
* Hard-coded constructor arguments (BBOB, PBO, CEC2022, DynamicBinVal); the registry name is
  `T(1,1).meta_data().name` (`factory.hpp`, `IdGetter`).
* CEC2013: ids hard-coded in the `InstanceBasedProblem::load_instances<T>` specializations
  (1101–1110 in each class header, 1111–1120 via `load_cf(...)` in `cec/2013/composition.hpp`);
  Shubert and Vincent have two entries each (fixed dims 2 and 3). Registry names are generated as
  `class_name<T>() + id` (e.g. `FivePeaks1101`, `CompositionFunction1111`) and **differ** from
  `meta_data().name` (`CEC2013FivePeaks`, `CEC2013CF1D2`); both are Python-visible and frozen.
* Star discrepancy: a formula (30/40/50 + index into a sample-size list) with explicit names
  (`UniformStarDiscrepancy10`).
* Graph: `default_id + index` over the non-blank lines of `static/example_list_*`; names
  `ClassName + id` (same as the meta name).
* CEC2013 and graph entries ignore both `instance` (always 1) and the requested dimension.
* `Factory::include(name, id, creator)`: a new name with a colliding id gets `max(ids) + 1`; an
  existing name keeps its id and **replaces the creator** (`wrap_problem` relies on this).
  Before WP3, `Factory::create` only `assert`ed that the key exists, so an unknown id/name was
  undefined behaviour in Release builds; since WP3 it throws `std::out_of_range`.
* Suites (8: Real, BBOB, SBOX, RealStarDiscrepancy, Integer, PBO, Submodular,
  IntegerStarDiscrepancy) got ids from the static-initialization order (not deterministic, not
  Python-visible); since WP3 they are fixed (Real 1, BBOB 2, SBOX 3, RealStarDiscrepancy 4; Integer 1,
  PBO 2, Submodular 3, IntegerStarDiscrepancy 4).
  Iteration order is problem id → dimension → instance.
* Not registered (constructed directly): WModel, funnel (`DoubleSphere`, ...), `ManyAffine`.
* MaxInfluence (2200–2223) is **stochastic**: `evaluate()` runs Monte-Carlo cascades with the global
  `ioh::common::random::GENERATOR`, seeded from `std::random_device` (`max_influence.hpp:61`,
  `random.hpp`). Only x = 0 has a reproducible value; infeasible points return a deterministic
  penalty. No other problem uses the global generator.
* Windows bug fixed in WP1: `graph_problem.hpp` built constructors with
  `{[graph, i](...){...}, i++, ...}`; MSVC captured the incremented `i`, so on Windows every graph
  problem's `meta_data().problem_id` was its registry id + 1.

**Registration sites before WP3** (since WP3 the built-ins are registered in `src/registry`, and these
bases register nothing; paths under `include/ioh/`): `problem/problem.hpp` (`RegisteredProblem`,
`AutomaticProblemRegistration`, `ProblemRegistry`/`ProblemFactoryType` aliases),
`problem/bbob/bbob_problem.hpp` (`BBOProblem`: registration only, 3 per BBOB function),
`problem/pbo/pbo_problem.hpp` (`PBOProblem`: registration + inherited constructors),
`problem/cec/2013/cec_problem.hpp` (`CEC2013Problem`: + `InstanceBasedProblem`),
`problem/cec/2022/cec_problem.hpp`, `problem/dynamic_bin_val/dynamic_bin_val.hpp`,
`problem/submodular/graph_problem.hpp` (`GraphProblemType`: + instance loading *implementation*),
`problem/star_discrepancy/{real,integer}.hpp`, `suite.hpp` (`AutomaticSuiteRegistration`,
`SuiteRegistry` alias). The aliases are used by `many_affine.hpp`, `example/`, `include/README.md`
and the C++ tests.

**Data files** (`static/`, 23 MB; `ioh/static` is a tracked symlink to `../static`): runtime data
`cec_transformations/2013` (`optima.dat`, `CF*_M_D*.dat`), `cec_transformations/2022`
(`F_i_star.txt`, `M_<f>_D<d>.txt`, `shift_data_<f>.txt`, `shuffle_data_<f>_D<d>.txt`; dims 2/10/20,
not all functions have D2), `example_graphs/`, `example_pwt/`,
`example_list_{maxcut,maxcoverage,maxinfluence,pwt}`; test fixtures `bbobfitness{5,20}.in`,
`pbofitness{16,100}.in`, `cec_problem{2013,2022}.in`, `dynamic_bin_val.in`; unused `conf.ini`.
`static.tar.gz` (repo root) duplicates `static/` and is downloaded by released versions.

**Python bindings** (`ioh/src/`): `ioh.cpp` (module), `problem.cpp` (base classes, `create`,
`problems`, `wrap_problem` registration via `Factory::include`), `real.cpp` (BBOB/SBOX, CEC2013,
CEC2022, star discrepancy, many-affine, funnel, DynamicBinVal), `integer.cpp` (PBO, WModel,
submodular), `problem_helpers.cpp` (Solution, State, constraints, bounds, MetaData, log info),
`logger.cpp` (incl. `logger.old` with the v1 Analyzer), `trigger.cpp`, `property.cpp`,
`empirical_attainment.cpp`, `real_suite.cpp`, `integer_suite.cpp`, `suite_helpers.cpp`, `suite.cpp`.
Numpy today: `Solution.x` is a writable view, bounds `lb`/`ub` and suite id/instance/dimension arrays
are copies, user callbacks (`wrap_problem` objectives, constraint functions) receive numpy arrays;
other vectors (e.g. logger `Info.x`) are lists.

The unfinished nanobind port lives on branch `origin/untemplating` (see WP6 for what to reuse).

---

## WP1 — Unblock CI and releases ✅ merged (#217)

What was done, for reference by later WPs:

* **T1.1 golden tests** — `tests/python/golden.py` (generator + shared helpers),
  `tests/python/test_golden.py`, fixtures in `tests/data/golden/` (generated from master 5338b27b).
  * Registry snapshot of every `ProblemClass` and all 96 class views, plus the ids `wrap_problem`
    assigns (REAL 1121, INTEGER 10005); it runs in a fresh interpreter because other tests register
    wrapped problems in-process.
  * 722 behaviour records: 2 instances × 2 dimensions for BBOB/SBOX/PBO/CEC2022/STAR_*, 2 instances
    at the fixed dimension for CEC2013, one record per GRAPH id (instance and dimension are
    ignored), DynamicBinVal step sequences, WModel/funnel/ManyAffine, and the iteration of all 8
    suites. Each record holds metadata, bounds, optimum, objective values at deterministic dense
    points (`random.Random(seed)`, stable across Python versions), at the optimum when it is finite,
    and, for binary problems, at sparse bit strings with 1/2/5 ones plus their constraint penalties
    (dense bit strings violate the budget of most graph problems, so their value is only the
    penalty; `--write` refuses fixtures without a feasible sparse point). MaxInfluence records only
    x = 0 and a finiteness check.
  * Comparison: names/integers exactly, floats with `rel_tol=1e-9` (objective values also with an
    absolute tolerance of `1e-9·|optimum_y|`, because raw + f_opt can cancel), containers by value.
  * Nothing touches `ProblemClass.problems`/`get_problem` on an empty GRAPH registry (that downloads
    data into the working directory). Run: `python tests/python/test_golden.py` (≈4 s).
* **T1.2 wheels** — `pypa/cibuildwheel@v4.3.3`, `[tool.cibuildwheel]` in `pyproject.toml`
  (CPython 3.10–3.15 plus free-threaded 3.14t/3.15t on Linux/macOS, skip win32 and musllinux,
  golden tests as
  `test-command`,
  `MACOSX_DEPLOYMENT_TARGET=10.15` because `setup.py` builds with
  `CMAKE_OSX_DEPLOYMENT_TARGET=10.15`, `CMAKE_BUILD_PARALLEL_LEVEL=3` on macOS), one job per
  OS × Python on native runners (`ubuntu-24.04`, `ubuntu-24.04-arm`, `windows-2022`,
  `windows-11-arm` from cp311, `macos-15-intel`, `macos-15`; the cp314/cp315 jobs also build the
  free-threaded variant), builds for every path that affects wheels (pull requests skip macOS; the
  full matrix runs weekly, on manual dispatch and for releases), superseded PR runs cancelled, upload
  only on `release: published`,
  no tag-deleting cleanup job, Dependabot for actions. Wheels contain only the `ioh` package (they
  used to install stray top-level `tests` and `doc` packages).
* **T1.3** — numpy, mypy, pybind11 removed from the build requirements; `python_requires>=3.10`;
  `MAKE_STUBS=1` builds now fail early with instructions when mypy is missing
  (`pip install mypy && MAKE_STUBS=1 pip install --no-build-isolation -e .`).
* **T1.4 push CI** — one `ci.yml` (replaces `ubuntu.yml`, `macos.yml`, `windows.yml`): a C++ job
  (Debug `test_ioh`; `ubuntu-24.04` with g++-12/13/14 and clang++-18, `macos-15`, `windows-2022`)
  and a Python job (`pip install -e .` + unittest on the three OSes, oldest and newest Python),
  each defined once with a matrix; runs on pushes to master, pull requests and manual dispatch;
  superseded PR runs are cancelled; `IOH_RESOURCES` for C++ test binaries; bounded parallelism
  (`--parallel 4` for C++, `CMAKE_BUILD_PARALLEL_LEVEL=3` for Python builds): unbounded jobs make
  the 3-vCPU/7 GB macOS runners swap for hours. README badges point to `ci.yml` and the wheels
  workflow.
* **T1.5** — googletest v1.10 → v1.18.0 (v1.10 does not configure with CMake ≥ 4); the submodule
  is pinned to the v1.18.0 commit and tracks the `v1.18.x` release branch, never `main`.
* Fixes needed to get green CI:
  * five C++ tests compared floating-point results exactly or with a fixed absolute tolerance and
    failed on arm64: optimum checks now use a relative tolerance (1e-12), the CEC2013 check 1e-10
    relative (worst arm64 deviation 1.7e-11), and logger output is compared with integers, layout
    and printed format exact and decimals within one unit of their last printed digit;
  * the Windows graph-id bug (§3);
  * `test_notebook_tutorial` used `functools.partial` as a class attribute, which binds `self`
    since Python 3.14.
* **T1.6** — no release after WP1 (decided, §1): the pipeline works again, but the next release is
  the final one (WP8). WP1's user-visible changes are listed in the WP8 release-notes draft.

---

## WP2 — Sobol direction numbers as a table ✅ T2.1 merged (#219), T2.2 done in WP3; T2.3 open

Depends on: WP1. Small and independent. T2.2 and T2.3 are later tasks recorded here; they do not
block WP3.

What was done, and where it differs from T2.1 below:

* The 13,224 assignments became `inline constexpr std::uint16_t sobol_direction_numbers[1111][13]`
  in `sobol.hpp`, generated by a script that checks every value lands where it was assigned. All
  values are below 2^13 and sit in columns 0–12, so the table takes 29 KB instead of the planned
  `long long [1111][62]` (551 KB). `i8_sobol` fills `v` from it, with zeros after a row's last
  direction number, instead of the zero loop plus the assignments; `<stdexcept>` (used) replaces
  `<exception>`.
* The reference output is pinned as hashes, because the points themselves (569k doubles) would be a
  fixture of several MB: `test_sobol_reference_output` in `tests/cpp/common/test_samplers.cpp`
  checks FNV-1a hashes of 64 consecutive points for dimensions {1, 2, 5, 10, 40, 100, 1111} and
  first seeds {0, 1, 7, 255, 1000, 4095, 65536}, recorded with the old implementation. A point is
  the XOR of the columns of V selected by the Gray code of its seed; without 255 and 4095 no hashed
  point would use table columns 7, 8, 11 and 12 (1,688 entries could change unnoticed).
* Fixed in passing (the old code had it too): an invalid dimension reset `v` before `i8_sobol`
  checked it and threw, so the next call with the previous dimension skipped re-initialization and
  returned wrong points. The check now comes first (`test_sobol_invalid_dimension`).
* Old and new side by side: 185 million coordinates and the returned seeds bit-identical (every
  dimension, 2^17 consecutive points in all 1111 dimensions, jumps forwards and backwards, other
  bounds, negative seeds).
* Measured (Apple M5, clang 21, -O3 without LTO): the TU of T2.1's acceptance (only `sobol.hpp`,
  emitting `i8_sobol`) compiles in 0.06 s instead of 5.7 s; a TU that uses the Sobol sampler through
  `sampler.hpp` in 0.22 s instead of 5.8 s (the rest is parsing). The Python extension build with
  `setup.py`'s flags (full LTO) takes 32 s instead of 41 s at -j4: about 1 s less per binding TU and
  6 s less LTO link (14.4 → 8.2 s). Debug builds barely change (the assignments were cheap at -O0).

### T2.1 Replace the assignment statements with one `static const` table

* **Context**: `include/ioh/common/sobol.hpp`, `i8_sobol()` (inline, line ~165) zero-initializes
  `static long long int v[DIM_MAX2][LOG_MAX]` (1111 × 62) and then runs 13,224 statements
  `v[i][j] = c;` (lines ~286–13521).
* **Steps**
  1. Before changing anything, record reference output of the current implementation (e.g. 64
     consecutive points for dims {1, 2, 5, 10, 40, 100, 1111} and seeds {0, 1, 7, 1000}) into a test
     fixture.
  2. Convert mechanically (script, not by hand): parse every `v[(\d+)][(\d+)] = (\d+);`, build a
     1111 × 62 table (zeros elsewhere; trailing zeros of a row may be omitted), emit
     `inline const long long int sobol_v_init[1111][62] = {...};` at namespace scope and replace the
     zero-init loop + assignments with `std::memcpy(v, sobol_v_init, sizeof(v));` (add `<cstring>`,
     `<stdexcept>`).
  3. Add a C++ (and/or Python) test comparing against the recorded output.
* **Acceptance**: outputs bit-identical; the TU containing `i8_sobol` compiles in < 0.2 s at -O3
  (was 5.6 s); golden tests pass. (Measured in the diagnosis: 324,864 values identical.)

### T2.2 Rename the wheels workflow ✅ done in WP3

* The wheels workflow was called "Upload Python Package", although it builds and tests wheels on
  every run and uploads only for releases. WP3, which runs the full wheel matrix anyway (§2), renamed
  it to "Wheels": only `name:` changed, because PyPI's trusted publishing is tied to the file name
  `python-publish.yml`.

### T2.3 Final CI timing (open, after all WPs)

* Once all WPs are merged, measure how long CI takes and compare with the baseline below: wall time
  and runner minutes of `ci.yml` and of the wheels workflow (a pull-request run and a full run), and
  the local build time of the Python extension (measured as in §2, compared with §0).
* Baseline. Wall time runs from the first job's creation to the last job's end, runner minutes add
  up the jobs' run times, and the organization runs at most 20 jobs at once, 5 of them on macOS.
  * WP1 (1075af4f): `ci.yml` (run 39651947896) 12 jobs of 2.4–7.3 min, 50 runner minutes, 7.4 min
    wall, no queueing. Wheels (run 39651947856; macOS still on pull requests then): 35 wheel jobs
    and the sdist, 225 runner minutes, 35.3 min wall, jobs queued up to 17 min.
  * #218 (dc644333), three runs started together: `ci.yml` (39736254606) 45 runner minutes,
    22.1 min wall; pull-request wheels (39736254587) 23 wheel jobs and the sdist, 108 runner
    minutes, 17.5 min wall; full wheels by manual dispatch (39736253932) 227 runner minutes, 41 min
    wall. The first two are upper bounds, because the full run competed for the slots.
  * WP2 (#219), the first pull-request runs without a parallel full run: `ci.yml` (39766818613)
    12 jobs of 2.9–5.7 min, 47 runner minutes, 5.7 min wall; pull-request wheels (39766818690) 23
    wheel jobs and the sdist, 94 runner minutes, 10.0 min wall, jobs queued up to 4.6 min.

---

## WP3 — Functional registry (must land as one unit) ✅ implemented (branch `wp3-functional-registry`)

Depends on: WP1, WP2. Goal: no TU except the registry TUs instantiates problem code; identical
Python-visible registry.

What was done, and where it differs from T3.1–T3.7 below:

* **Compiled target**: `ioh` is a static library of `src/registry/*.cpp` (PIC, `PUBLIC` usage
  requirements, install/export as before). The `/MT` `MSVC_RUNTIME_LIBRARY` property was dropped: on a
  compiled library it would clash with the `/MD` Python extension and tests. `MANIFEST.in` grafts
  `src/`.
* **Build details from review**: `CMAKE_BUILD_TYPE` defaults to Release for top-level single-config
  builds (the installed library holds problem code; `cmake .. && make install` built it at -O0); the
  Debug library is `iohd` (Debug and Release installs coexist; consumers on MSVC need the matching
  one); `/MP` for `ioh` under the Visual Studio generator (which `setup.py` uses on Windows); `src/**`
  in the wheels workflow's path filter, and the workflow renamed to "Wheels" (T2.2).
* **`IOH_LTO`** (T3.6) applies to the release configurations only. It is on by default only for the
  Python extension with MSVC (`/GL`), as planned: MSVC then compiles without generating code, and without
  it the Windows wheel jobs built slower than before WP3 (the build step of a Windows x64 wheel took
  145 s instead of 92 s with pybind11's `/GL`). Elsewhere it is off: with clang, evaluation speed is the
  same with full LTO (master), no LTO and ThinLTO, ThinLTO builds only slightly faster (19.7 instead of
  20.9 s at -j4), and a static library of LTO objects only links with the toolchain that built it.
  Defining `CMAKE_INTERPROCEDURAL_OPTIMIZATION` also turns off pybind11's own full LTO.
* **Measured** (Apple M5, clang 21, §2): the Python extension build with `setup.py`'s flags takes 20.9 s
  instead of 30.9 s at -j4 (62.4 instead of 81.3 s serial); `pip install` of the sdist 19 s; Debug
  `test_ioh` 18.8 instead of 23.5 s at -j4. Runtime benchmark (BBOB d=10/200, PBO d=1024, CEC2022
  d=10, import) unchanged.
* **Factory**: `add_builtins` overloads for the 13 factories with built-ins (the 11 problem factories of
  §3 plus the two suite factories) are declared, with forward declarations of their types, before
  `Factory` in `factory.hpp`, and defined in `src/registry`; a no-op template covers every other
  factory. Duplicate built-ins throw `std::logic_error`, unknown ids/names `std::out_of_range`
  (Python: `IndexError`); runtime `include()` keeps its Release semantics in all builds (the
  Debug-only assert on an existing name is gone); a recursive mutex guards the maps and creators run
  outside it.
* **Ids and names** come from the same sources as before: `T(1, 1).meta_data()` or `load_instances`,
  via `RegisterWithFactory::include<T>(factory)`. So no `static constexpr` id/name members were needed
  (T3.3), and the family lists in `src/registry/families.hpp` are plain type lists.
* **CRTP**: the library's types no longer use the automatic registration. The former registering
  bases (`BBOProblem`, `PBOProblem`, `CEC2013Problem`, `CEC2022Problem`, `DynamicBinValProblem`,
  `GraphProblemType`, the star discrepancy classes, `RealSuite`/`IntegerSuite`) keep their real bases
  and constructors but register nothing, so no problem header changed apart from the BBOB `extern
  template` lines and no deprecated aliases were needed. The machinery itself stays for user types
  (`RegisteredProblem`, `AutomaticProblemRegistration`); new `register_problem<T, Parent>()` and
  `register_problem<T, Parent>(id, name)`, whose created problems report the id and name they are
  registered under (a taken id becomes the next free one), unlike `RegisteredProblem`'s.
  Classes derived from the former registering bases are therefore no longer registered (a silent
  C++ change, in the release notes).
* **Suites**: `Suite`'s constructor takes any problem factory (a template), so the six
  `reinterpret_cast`s are gone. Fixed ids: Real 1, BBOB 2, SBOX 3, RealStarDiscrepancy 4; Integer 1,
  PBO 2, Submodular 3, IntegerStarDiscrepancy 4.
* **`fixed_dimension` / `fixed_instance`** (T3.7): virtual accessors on `Problem` (overridden by
  `CEC2013` and `GraphProblem`) and read-only Python properties on every problem class. The golden
  behaviour records got the two keys (714 of 722 records; the existing values were reproduced
  exactly). Registry entries do **not** carry them yet: a graph's dimension is only known after
  loading its data (all 78 graphs take 0.8 s), so this moves to the WP4 manifest.
* **Tests**: `tests/cpp/problem/test_registry.cpp` checks every factory against the golden registry
  (ids, names, CEC2013 meta names from the golden records, fixed values, construction and evaluation;
  the nine family factories exactly, the REAL/INTEGER unions allowing runtime registrations after the
  built-ins and constructing one graph entry per graph class), the suite ids, unknown and duplicate
  entries, and `register_problem`.

### T3.1 Compiled `ioh` target

* Convert the top-level `ioh` target from `INTERFACE` to a `STATIC` library with sources under a new
  `src/` directory (registry and, in WP4, data code); keep `ioh::ioh` alias, include dirs, fmt
  dependency, install/export rules (`ioh-targets.cmake`). The Python module (`ioh/CMakeLists.txt`),
  tests and examples link it. Build with `POSITION_INDEPENDENT_CODE ON` (it is linked into the
  Python extension).

### T3.2 Factory API

* Keep the public surface used by the bindings and documented C++ code: `Factory<AbstractType,
  Args...>::instance()`, `include(name, id, creator)`, `create(name|id, args...)`, `names()`,
  `ids()`, `map()`, and the aliases `ProblemRegistry<T>`, `ProblemFactoryType<T>`,
  `SuiteRegistry<T>`, `SuiteFactoryType<T>`.
* `instance()` populates the factory lazily, in place (Factory is not copyable or movable):

  ```cpp
  static Factory &instance() { static Factory f{populate_tag{}}; return f; }
  private: explicit Factory(populate_tag) { add_builtins(*this); }
  ```

  `add_builtins` is a set of overloads (or a trait) **declared** in a header and **defined
  out-of-line** in the registry TUs — never inline in a header (that would bring the per-TU cost
  back). Factories without built-ins use a no-op default.
* A missing registry must be a **link error**, never an empty factory (the hard reference from the
  factory also prevents static-archive dead stripping).
* Built-ins are added inside the factory's own construction, before any runtime registration, so ids
  are deterministic. Never call `instance()` re-entrantly from `add_builtins`.
* Duplicate id or name among built-ins → hard error. Runtime registrations keep today's semantics
  (colliding id → `max+1`; existing name → creator replaced, id kept).
* `create()` with an unknown id/name must throw (`std::out_of_range`), not assert.
* Add a mutex around `include()` and lookups (runtime registration from Python).

### T3.3 Family lists and pinned ids/names

* One list per family (type list or X-macro) written once: BBOB (24 templates, instantiated for
  `BBOB` and `SBOX`), PBO (25), CEC2022 (12), DynamicBinVal (4), star discrepancy (generated
  entries), CEC2013 and graph (data-backed). Every factory is composed from these lists: `BBOB` ←
  BBOB<BBOB>; `SBOX` ← BBOB<SBOX>; `REAL` ← BBOB<BBOB> + CEC2013 + CEC2022 + star-real; `INTEGER` ←
  PBO + graph + star-integer + DynamicBinVal; etc. (§3 has the exact contents).
* Class-backed problems get their id/name as `static constexpr` members used by both their
  constructor and the registry, so direct construction
  (`ioh.iohcpp.problem.Sphere(1, 5).meta_data.problem_id == 1`) is unchanged.
* CEC2013 is pinned per registry entry (id, registry name, meta name, dimension) — not per class
  (Shubert/Vincent have two entries; registry and meta names differ, see §3).
* Data-backed and generated families (CEC2013, star discrepancy, graph) keep their current loaders
  in this WP, wrapped as callables that run when their factory is first populated (no longer at
  static init). WP4 replaces them with manifest entries.

### T3.4 Remove the CRTP machinery

* Delete `AutomaticTypeRegistration`, `RegistrationInvoker`, `InvokeApplyOnConstruction`,
  `IdGetter` (factory.hpp) and `AutomaticProblemRegistration`/`AutomaticSuiteRegistration`.
  `RegisterWithFactory` may go, but keep the public aliases (T3.2) working.
* Replace the registering bases: `BBOProblem` (registration only) and `PBOProblem` (registration +
  inherited constructors) → problems derive from `BBOB`/`SBOX`/`PBO` directly; keep the old names as
  deprecated non-registering aliases for one release. `CEC2013Problem`, `GraphProblemType`,
  the CEC2022/DynamicBinVal/star bases: drop only the registration bases and keep any
  implementation they carry (`GraphProblemType` contains the instance loading used until WP4).
* BBOB headers: replace `template class Foo<BBOB>;` by
  `extern template class Foo<BBOB>; extern template class Foo<SBOX>;` and put the explicit
  instantiation definitions in the BBOB registry TU.
* Provide `ioh::problem::register_problem<T, Parent>(id, name)` (thin wrapper over `include`) and a
  deprecated `RegisteredProblem` shim for user types (it may keep a static-init registration for
  *user* types only — cheap, because only the user's TU instantiates it).
* Update `example/` and `include/README.md`.

### T3.5 Suites

* Register the 8 suites explicitly, in a fixed order (their ids are not Python-visible). Remove the
  `reinterpret_cast<Factory&>(...)` of problem factories in `include/ioh/suite.hpp` (undefined
  behaviour); give suites a creator callable that upcasts. Suite behaviour is frozen by the golden
  `suite/*` records.

### T3.6 Build options

* CMake option `IOH_LTO` (default OFF for GCC/Clang, ON for MSVC until measured on CI); set
  `CMAKE_INTERPROCEDURAL_OPTIMIZATION` from it (defining it disables pybind11's own LTO). Keep -O3.
* Report build times before/after (protocol in §2), including CI wheel times per platform.

### T3.7 Tests

* Golden tests unchanged and green on all platforms.
* C++ test: every factory's `{id: name}` equals `tests/data/golden/registry.json`; every entry
  constructs, its `meta_data().problem_id` equals the registry id, its meta name equals the registry
  name (except CEC2013, see §3 — compare with the golden meta names), and it evaluates.
* Test that `create()` with an unknown id/name throws.
* Expose `fixed_dimension` and `fixed_instance` (decided, §1; in the same PR as the registry,
  decided): registry entries carry them, and Python problems get read-only properties with the same
  names (`None` for problems that honour the requested dimension/instance). Values for the
  data-backed families come from the WP4 manifest; until then they are filled in from the current
  loaders. Add them to the golden fixtures in the same PR (new keys only; existing values must not
  change).

---

## WP4 — Data layer and manifest

Depends on: WP3.

### T4.1 Data root API

* New `include/ioh/common/data.hpp` + `src/data.cpp`: `ioh::data::set_root(path)`, `root()`,
  `file(relative)`. Resolution order: `set_root` > `IOH_RESOURCES` > compile-time default from CMake
  (`IOH_DATA_DIR`: the source tree's `static/` for in-repo builds/tests, `share/ioh` for installed
  builds). Remove the current-directory ancestor scan from `get_static_root()`. Missing root or file
  → exception (`ioh::data::data_error`) listing where it looked and how to fix it.

### T4.2 Manifest

* Script (e.g. `tools/gen_manifest.py`) generating a checked-in `static/manifest.json`: every data
  file with relative path, size, sha256, plus `DATA_VERSION` (handle the `ioh/static` symlink and
  paths with spaces). It also lists the data-backed registry entries (graph, pack-while-travel,
  CEC2013): pinned id, registry name, meta name, files, fixed dimension — generated once from the
  golden registry and the definition files; ids are never again derived from file line order.
* Registry entries get `fixed_dimension` / `fixed_instance` from the manifest (moved here from T3.7:
  without it a graph's dimension is only known after loading the graph). Problems already report them
  since WP3; the registry values must equal what the constructed problems report.
* CI check: wheel contents (and the source tree) match the manifest (paths + sha256).

### T4.3 Lazy, cached datasets

* `ioh::data::cached<Key, T>(key, loader)`: loads once (`std::call_once` or equivalent), shared
  across problem instances, thread-safe.
* Convert CEC2022 (`F_i_star.txt`, `M_<f>_D<d>.txt`, `shift_data_<f>.txt`,
  `shuffle_data_<f>_D<d>.txt`), CEC2013 (`optima.dat`, `CF*_M_D*.dat`), graph and pack-while-travel
  loading. Remove the silent fallback in `load_shuffle_data` (`if (exists(...))`): whether a shuffle
  file exists is declared by the manifest; a declared-but-missing file is an error.

### T4.4 One pattern for instance-based problems

* Remove `InstanceBasedProblem` and `load_instances`. CEC2013, star discrepancy, graph and
  pack-while-travel become plain registry entries (id, name, `create(instance, dim)`), data loaded on
  `create`. Keep today's tolerance: these problems ignore `instance`/`dim` where they do now.
* Keep runtime loading of user graph definition files (`load_graph_instances(path)` and the Python
  class methods `MaxCut.load_instances(...)` etc.) as callable registration with today's id
  semantics.

### T4.5 Python side

* At import: `iohcpp.set_data_root(importlib.resources.files("ioh") / "static")` unless
  `IOH_RESOURCES` is set (keep honouring it, and keep the `ImportError` when it points to a
  directory that does not exist). Stop requiring the environment variable otherwise.
* `download_static_folder()` and `load_graph_problems()`: deprecated no-ops with
  `DeprecationWarning` (graph problems are always registered now). Update
  `tests/python/test_downloader.py`.
* A missing or incomplete data file must raise a Python exception at `get_problem` time (test it),
  never abort the interpreter.

### T4.6 Repository and packaging

* Move test fixtures (`*fitness*.in`, `cec_problem20xx.in`, `dynamic_bin_val.in`) to
  `tests/data/` and update C++/Python tests (C++ tests get the path via a CMake compile definition).
* Delete `static/conf.ini` (unused). Keep `static.tar.gz` (decided, §1: released versions download
  it) and add a CI check that its contents equal `static/` (rebuild it in the same PR when data
  changes).
* `install(DIRECTORY static/ DESTINATION ${CMAKE_INSTALL_DATADIR}/ioh)`; make sure the sdist and
  wheels contain exactly the manifest's files. Coordinate with T5.1, which takes over packaging.

---

## WP5 — Build system

Depends on: WP3 (compiled target) and WP4 (packaging contents).

### T5.1 scikit-build-core

* Replace `setup.py` with scikit-build-core (`pyproject.toml` `[project]` + `[tool.scikit-build]`):
  persistent `build-dir` (e.g. `build/{wheel_tag}`) and editable rebuilds (no more
  `shutil.rmtree(build)`), Ninja, clang++ (not `CXX=clang`), `wheel.packages = ["ioh"]` (WP1
  already stopped installing stray top-level `tests`/`doc` packages), static data installed into `ioh/static`,
  macOS deployment target ≥ 10.15 (set via `MACOSX_DEPLOYMENT_TARGET`, keep in sync with
  `[tool.cibuildwheel.macos.environment]`), ARCHFLAGS mapped to `CMAKE_OSX_ARCHITECTURES` (enables
  building x86_64 macOS wheels on arm64; do not overwrite ARCHFLAGS like `setup.py` does today).
* Do **not** set `wheel.py-api` (abi3) here: that is only correct once the module is built with
  nanobind's `STABLE_ABI` (T6.7).
* Move the docs/stubs hooks: `MAKE_DOCS` runs via `atexit` in `setup.py`, `MAKE_STUBS` runs in
  `CMakeBuild.run()` after the extension is built; turn both into explicit scripts or CI steps.
* List the sdist contents explicitly (`sdist.include` / `sdist.exclude`): today the `*.md` in
  `MANIFEST.in` also ships `TASKS.md`, which must not be in the 0.4.0 sdist.

### T5.2 CI build speed

* Windows: use Ninja or add `/MP` (the Visual Studio generator compiles the module's TUs serially).
* ccache/sccache for push CI only (binding TUs include `Python.h`, so caches don't help across
  Python versions in release builds).

### T5.3 C++ tests

* `tests/CMakeLists.txt` builds every test source twice (`test_ioh` + one executable per file) and
  calls `gtest_discover_tests` twice per target: build one test executable by default (per-file
  executables behind an option), discover tests at test time (`DISCOVERY_MODE PRE_TEST`; the
  binaries still need the data root, which WP4 provides via the compiled default).
* CMake minimum: `BUILD_TESTS` defaults to ON, and googletest 1.18 needs CMake ≥ 3.16, but
  `CMakeLists.txt` and `include/README.md` say 3.15. Raise both to 3.16, or default `BUILD_TESTS` to
  ON only when IOHexperimenter is the top-level project.

---

## WP6 — Move bindings to nanobind

Depends on: WP3, WP4, WP5.

### T6.1 Build integration

* Find nanobind via pip build requirement; `nanobind_add_module(iohcpp NOMINSIZE ...)`, link the
  `ioh` static library. Remove the `external/pybind11` submodule and all pybind11 references.

### T6.2 Port the binding TUs

* Start from `origin/untemplating` `ioh/src/*.cpp` as reference for nanobind idioms, but **do not**
  take its header changes (commented-out registration, commented-out `IOH_DBG`, redesigned
  CEC2013/star/submodular classes), its PCH, or its `setup.py`. That branch's registry is empty
  (`get_problem` fails for every class), it only compiles with `NDEBUG`, and its CI fails on Windows.
* Generate per-problem class bindings from the WP3 family lists (no hand-maintained duplicate list).
* Keep module layout (`ioh.iohcpp.problem`, `.suite`, `.logger` and its submodules incl. `old`),
  class/method names, signatures, docstrings and default arguments (the branch changed some:
  `Analyzer(root=...)` must stay `fs::current_path()`, `FlatFile(output_directory="./")`).
* Write binding sources unity-safe (no file-scope `using namespace`, unique helper names); enable a
  unity build only if it measurably helps.

### T6.3 Array inputs

* For `__call__` (1-D and batched 2-D) and every method that takes vectors: add
  `nb::ndarray<const double, nb::ndim<1>, nb::c_contig, nb::device::cpu>` (and integer
  equivalents) overloads **registered before** the `std::vector` overloads. Cause of the branch's
  slowdown: `np.float64` is not an exact float, so with two `__call__` overloads nanobind's strict
  pass fails and each attempt converts the array through `PySequence_Tuple` (3 conversions per call
  vs 1 in pybind11). Lists must keep working.

### T6.4 API gaps found in the branch (must be closed)

* `MaxCut/MaxCoverage/MaxInfluence/PackWhileTravel.load_instances` (now WP4's runtime
  registration), `FlatFile.watch`, `Store.watch`, star-discrepancy `grid` property, the `Path`
  class and implicit str → path conversion (`nanobind/stl/filesystem.h`), pickling
  (`__getstate__/__setstate__`) of triggers/properties, copy-vs-view semantics of arrays.
  `tests/python/test_array_layout.py` only compares values: first add a test that pins whether
  `Solution.x` is a writable view that aliases C++ memory (it is on master).

### T6.5 Trampolines, stubs, platforms

* Trampolines (`NB_TRAMPOLINE`/`NB_OVERRIDE`) for Python-subclassable classes: watchers/loggers,
  properties, triggers, constraints, wrapped problems.
* Stubs: replace mypy `stubgen` in `doc/generate_docs.py` with `python -m nanobind.stubgen`;
  regenerate `ioh/iohcpp/*.pyi` and `ioh/iohcpp/logger/*.pyi` (and add one for `logger.old`).
* Debug builds compile; Linux (x86_64, aarch64), Windows (x64, ARM64) and macOS (Intel, arm64) green.

### T6.6 Free-threading

* The free-threaded wheels built since WP1 re-enable the GIL on import, because the module does not
  declare that it is thread-safe. After the thread-safety audit (factory mutex from T3.2, loggers
  and triggers, the global random generator used by MaxInfluence, the WP4 data cache) build with
  nanobind's `FREE_THREADED` so the GIL stays disabled, and add a multi-threaded evaluation test.
  Also remove the `cp3??t-win*` skip in `pyproject.toml`: nanobind finds the free-threaded
  `python3XXt.lib` on Windows, the pinned pybind11 2.11 does not.

### T6.7 Stable ABI

* `STABLE_ABI` for CPython ≥ 3.12 + scikit-build-core `wheel.py-api = "cp312"` (only for those
  builds): per platform one cp310, one cp311 and one cp312-abi3 wheel. cibuildwheel only reuses an
  abi3 wheel within the same run, so restructure the WP1 sharding: one job per OS with
  `CIBW_BUILD="cp312-* cp313-* cp314-* cp315-*"` (builds once, tests on all four) plus the cp310 and
  cp311 jobs. abi3audit runs by default in cibuildwheel 4. Free-threaded wheels cannot be abi3 and
  stay version-specific.

### T6.8 Acceptance

* Golden tests green on all platforms; runtime at least at parity with pybind11 for numpy *and*
  list inputs (target: faster); import time not worse; build time reported.

---

## WP7 — Return numpy arrays instead of lists

Depends on: WP6.

* **Context**: see §3 for what already returns numpy (`Solution.x` view, bounds copies, suite
  arrays, callback arguments) and what returns lists (e.g. logger `Info.x`).
* **Steps**: audit every bound member/method returning a vector; return `numpy.ndarray`
  (float64 for real, integer dtype for integer problems). Per field choose: read-only zero-copy
  view with the owner kept alive (internal state), writable view (only where mutation is meant to
  propagate, e.g. `Solution.x` today), or copy (unclear lifetime).
* **Docs**: upgrade-guide entries for list concatenation (`+` now adds), `==` becoming elementwise,
  JSON serialization (`.tolist()`), `isinstance(x, list)` checks.
* **Acceptance**: golden values unchanged (they compare by value); type expectations in tests
  updated.

---

## WP8 — Documentation and release

Depends on: all previous WPs (each WP also documents its own changes as it lands).

* C++ upgrade guide: link the compiled `ioh::ioh` target (every program, also one that only
  constructs problems: the BBOB classes are instantiated in the library); registration via
  `register_problem` / `include`; classes derived from the former registering bases (`BBOProblem`,
  `PBOProblem`, `CEC2013Problem`, `CEC2022Problem`, `DynamicBinValProblem`, `GraphProblemType`,
  `RealSuite`, `IntegerSuite`) must register themselves, while `RegisteredProblem`,
  `AutomaticProblemRegistration` and `AutomaticSuiteRegistration` still register at program start;
  `InstanceBasedProblem` removed (WP4); data-root API; errors are exceptions instead of aborts;
  removed helpers/types list.
* Python upgrade guide: Python ≥ 3.10 (recommended ≥ 3.12), deprecated `download_static_folder` /
  `load_graph_problems`, `IOH_RESOURCES` semantics, numpy returns (WP7), new metadata attributes if
  added.
* README/docs: installation, building from source (scikit-build-core), data location; changelog
  (from the release-notes draft below).
* Release, the first since 0.3.22 (§1): version bump to 0.4.0; run the wheels workflow on master by
  manual dispatch and check the artifacts (take the wheel tags in the release notes from this run;
  no `TASKS.md` in the sdist); then publish the GitHub release (`release: published` triggers the
  upload) and verify the wheels on PyPI.
* Final CI timing (T2.3).

### Release-notes draft

Each WP adds its user-visible changes here (§2).

* Since 0.3.22, before WP1: ManyAffine (MA-BBOB) evaluates faster by skipping components with weight
  0; their `sub_problems` are no longer evaluated, so their state no longer changes (#216).
* WP1:
  * Python ≥ 3.10 (3.9 dropped).
  * Linux x86_64 wheels are tagged `manylinux_2_27/2_28` (glibc ≥ 2.27; 0.3.22: manylinux2014,
    glibc ≥ 2.17), the new aarch64 wheels `manylinux_2_26/2_28` (glibc ≥ 2.26). No more i686 Linux
    and 32-bit Windows wheels: there, and on x86_64 Linux with glibc < 2.27, pip builds 0.4.0 from
    source (C++17 compiler and CMake needed); `pip install "ioh<0.4"` keeps the 0.3.22 wheels.
    macOS tags are unchanged (Intel 10.15, arm64 11.0).
  * New cp313–cp315 wheels; new Linux aarch64 and Windows ARM64 (3.11+) wheels.
  * Free-threaded 3.14t/3.15t wheels for Linux and macOS (WP6 updates this entry: Windows wheels,
    and the GIL stays disabled).
  * Wheels no longer install top-level `tests`/`doc` packages; the sdist no longer contains the
    Python tests.
  * Graph problems on Windows: `meta_data().problem_id` and the problem name were one too high
    (`get_problem(2308)` was named `PackWhileTravel2309`), also in logged data.
  * C++: the default configure (`BUILD_TESTS=ON`) works with CMake 4 again (googletest 1.18) and
    needs CMake ≥ 3.16 (T5.3 settles the documented minimum).
* WP2: Sobol sampler (`SobolStarDiscrepancy*` problems, `StarDiscrepancySampler.SOBOL`, C++
  `i8_sobol` and `sampler::Sobol`): after a call with a dimension outside 1–1111 raised an error
  (`ValueError` in Python), later points in the previous dimension were ≈ 0 until another dimension
  was used. Fixed.
* WP3:
  * New: `fixed_dimension` and `fixed_instance` on every problem (Python properties, C++ methods): the
    dimension and instance a problem always uses whatever was requested (CEC2013 and graph problems),
    `None` otherwise.
  * `create()` with an unknown id or name raises `IndexError` in Python (`std::out_of_range` in C++)
    instead of failing an assertion or, in release builds, undefined behaviour.
  * C++: the library is no longer header-only. Every program links `ioh::ioh` with CMake (or compiles
    `src/`): it holds the registries of the built-in problems and suites, which are filled when a
    factory is first used instead of at program start, and the BBOB/SBOX problem classes. The default
    build type is Release; the Debug library is `iohd`. Register your own problems with
    `ioh::problem::register_problem` (`RegisteredProblem` still registers at program start). Suite
    factory ids are now fixed (Real 1, BBOB 2, SBOX 3, RealStarDiscrepancy 4; Integer 1, PBO 2,
    Submodular 3, IntegerStarDiscrepancy 4).
  * C++ (breaking, without a compile error): classes derived from the library's former registering
    bases (`BBOProblem`, `PBOProblem`, `CEC2013Problem`, `CEC2022Problem`, `DynamicBinValProblem`,
    `GraphProblemType`, `RealSuite`, `IntegerSuite`) are no longer added to any factory, so
    `create(id|name)` throws. Register them with `register_problem<T, Parent>()` for each factory the
    base used to fill (e.g. `PBO` and `IntegerSingleObjective`), or `SuiteRegistry<...>::include<T>()`
    for suites.
  * Problem ids, names and behaviour are unchanged (golden tests).

---

## Later / optional

* Turn the BBOB/SBOX template parameter into a runtime flag (halves the BBOB classes).
* Use fmt as a compiled library instead of header-only.
* Move more implementation out of headers into the compiled target.
* Make MaxInfluence reproducible (instance-seeded generator instead of the global one).
