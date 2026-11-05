# WP E03: Suite registry export
Branch: wp/E03-suite-registry
Status: in progress

## Done
- step 1/3: `ioh/suite_registry.py` with `registry()` (also `ioh.registry()`), `scoring()`, `python -m ioh.suite_registry -o FILE` JSON export; the release workflow (build_sdist job) writes registry.json, uploads it as an artifact and attaches it to the release.
- step 2/3: leaderboard fields per suite: standard budget, standard instances and dimensions, target bounds, scoring (AOCC; per-problem overrides via `SCORING_OVERRIDES`, `problems[].scoring`).
- step 3/3: pending (tests, see below)

## Next step
Step 3: tests/python/test_registry.py.

## Decisions
- Structure of the problems (ids, names, direction, bounds, optima) is read live from the installed package, so it cannot drift. The conventions are a table in `SUITE_CONVENTIONS`.
- The module is `ioh.suite_registry`; `ioh.registry()` is a lazy wrapper (a module named `registry` would clash with the function).
- Suites listed: BBOB, SBOX, PBO, Submodular, RealStarDiscrepancy, IntegerStarDiscrepancy. The generic `Real` and `Integer` suites (all problems) are not benchmark suites and are left out. CEC problems are not in a suite class, so not included.
- The ranges of instances/dimensions are checked against include/ioh/suite.hpp by the test (the C++ only asserts them).
- Unknown optima (graph, star discrepancy: C++ reports inf) are `null` with `known_optimum: false`.
- No new dependency. Python module built in `.venv` (pip install --no-build-isolation .), tests run from /tmp (`python tests/python/test_registry.py`) so the installed package is used.

## Open questions / Blocked
- PROVISIONAL conventions, to be confirmed by the leaderboard owner (W13/D13): budgets (BBOB/SBOX 10000*D, PBO 100*D, others null), standard instances (1-5) and dimensions, target bounds (BBOB/SBOX 1e-8..1e2 precision, log; PBO linear from 0, upper null; Submodular and star discrepancy unset).
- The release attach step (`gh release upload`) is untested; needs a real release.
