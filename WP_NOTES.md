# WP E07: Remove the v1 Analyzer logger
Branch: wp/E07-remove-analyzer-v1
Status: ready for review

## Done
- step 1/3: folded analyzer::v1 and v2 into one analyzer::Analyzer, removed the .info writer and its C++ tests; 118 C++ tests pass (Debug).

- step 2/3: removed ioh.logger.old, Experiment(old_logger=), stubs, tests, tutorial, example README.
- step 3/3: CHANGELOG ### Removed and upgrade guide sections. Python module built in .venv; all Python tests pass in a
  copy outside the tree (test_golden.py and test_logger.py included). The one error `test_array_layout.py::
  tester_inc_one_transform` is pytest collecting a helper function, unrelated.

## Next step
None. Controller: push once E05 is merged and open the draft PR.

## Decisions
- No `analyzer::v2::Analyzer` alias: the class is `ioh::logger::analyzer::Analyzer`, `ioh::logger::Analyzer` is an alias to it.
- Dropped the `use_old_data_format` constructor argument with v1.
- Kept E05's `full_precision` and "precision" JSON key.

## Open questions / Blocked
- none
