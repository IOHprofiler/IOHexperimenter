
Note on tests interface
=======================

All tests come with extended logging capabilities.
If you need to debug some specific part of the IOH library, you can start by enabling
more information about what is actually going on.
To do so, you just have to pass a few arguments to any test executable.

Usage: `<test> [--gtest* options] [level [scope [depth]]`

- Level is a string indicating the log threshold, levels are (in asscending
order of importance, default: Warning): 
    - Critical,
    - Error,
    - Warning,
    - Progress,
    - Note,
    - Info,
    - Debug,
    - XDebug.
- Scope is an ECMAscript regexp matching the source files (default: none, do
  not forget the quotes).
- Depth is an integer indicating the call stack threshold (default: none).

For example, this is a valid call:
`./tests/test_eaf --gtest_filter=*eaf_single_level XDebug`
which is equivalent to:
`./tests/test_eaf --gtest_filter=*eaf_single_level XDebug '.*' 999`

To get a list of available GoogleTest options, use the `--help` argument.
In particular, the passing the `--gtest_list_tests` will give you the list of
all available tests.

For more information, you can also check the [clutchlog library manual](https://nojhan.github.io/clutchlog/)
and the [GoogleTest documentation](https://google.github.io/googletest/).

