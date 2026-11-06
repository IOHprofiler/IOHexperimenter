# WP E07: Remove the v1 Analyzer logger
Branch: wp/E07-remove-analyzer-v1
Status: in progress

## Done
- step 1/3: folded analyzer::v1 and v2 into one analyzer::Analyzer (analyzer.hpp), removed .info writer, C++ tests
  for v1; all 118 C++ tests pass (Debug).

## Next step
Python side (steps 2-3): bindings, Experiment, stubs, docs, CHANGELOG are edited; build the module and run all
Python tests incl. test_golden.py; then set Status to ready for review.

## Decisions
- No `analyzer::v2::Analyzer` alias: the class is `ioh::logger::analyzer::Analyzer`, `ioh::logger::Analyzer` is an alias to it.
- Dropped the `use_old_data_format` constructor argument with v1.
- Kept E05's `full_precision` and "precision" JSON key.

## Open questions / Blocked
- none
