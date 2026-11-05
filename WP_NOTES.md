# WP E02: FlatFile separator bug
Branch: wp/E02-flatfile-separator
Status: in progress

## Done
- step 1: failing tests (meta data on: run and first property glued, e.g. "01"; meta data off passes) in tests/cpp/logger/test_flatfile.cpp

## Next step
Step 2: fix include/ioh/logger/flatfile.hpp, changelog entry.

## Decisions
- Test target: `test_flatfile` (cmake -DIOH_TEST_PER_FILE=ON -DCMAKE_BUILD_TYPE=Debug), build dir `build-e02` (excluded via .git/info/exclude). Run: `build-e02/tests/test_flatfile Warning ".*" 99999` (the arguments are needed). Release (NDEBUG) builds crash in SetUp (clutchlog no-op `logger()` returns nothing, a separate pre-existing issue, not fixed here).
- Submodules fmt and googletest were initialised in the worktree (`git submodule update --init`).

## Open questions / Blocked
- none
