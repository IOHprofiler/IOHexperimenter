# WP E05: Full-precision output
Branch: wp/E05-full-precision
Status: ready for review

## Done
- step 1/3: C++ option `FlatFile::set_full_precision(bool)` (also on Analyzer), writing `{:.17g}` for properties with the default double format and for x positions; JSON metadata key `"precision": "%.17g"`; tests `flatfile_full_precision`, `analyzer_full_precision` in tests/cpp/logger/test_flatfile.cpp
- step 2/3: Python property `full_precision` (read/write) on FlatFile and Analyzer, stubs, tests `test_full_precision` and `test_flatfile_full_precision` in tests/python/test_logger.py (15 passed, run from a temp dir against `.venv` build)
- step 3/3: CHANGELOG entry under `## Unreleased`

## Next step
Nothing; review and merge (E02 and E03 also add `## Unreleased` to CHANGELOG.md: expected conflict).

## Decisions
- The default stays `{:.10f}` in this package. The option is off by default.
- Applies to properties whose format equals DEFAULT_DOUBLE_FORMAT, and to the x columns. Properties with an explicit format (evaluations `{:d}`, user formats) keep it. Python subclasses overriding `call_to_string` are bypassed for default-format properties when the option is on (FlatFile reads the value through `operator()`).
- Old `.info` files (v1 analyzer) still use `{:g}` for best y; not changed.
- JSON metadata: key `"precision": "%.17g"` is written only with the option on. Absence means the legacy `%.10f`. Old readers ignore unknown keys.
- How to flip the default later: change the `full_precision_` initialiser in flatfile.hpp to true (and keep the setter for opting out), in a release that announces it in the changelog, once iohinspector and IOHanalyzer readers are known to read the longer values. Then also write the `"precision"` key always (`"%.10f"` for the legacy mode).
- Set the option before the first problem is attached (JSON metadata is created then).

## Open questions / Blocked
- none
