## Source codes of IOHexperimenter

[This folder] includes common used codes of __IOHexperimenter__. For details of `IOHprofiler_problem` and `IOHprofiler_suite`, please visit [problem page](src/Problems) and [suite page](src/Suites).

### IOHprofiler_transformation

IOHprofiler_transformation` defines methods of transformation applied on `IOHprofiler_problem`. For the definition of methods, please visit [wikipage](https://iohprofiler.github.io/Benchmark/Transformation/)

### IOHprofiler_random

__IOHexperimenter__ supplies methods of generating random numbers, which can be used to reproduce same experiments with the same random seed.

`IOHprofiler_random` is a `class` of random methods, you can construct it with a specified random seed as:
```cpp
IOHprofiler_random(long seed);
```

And two random number generator methods are as follow.
* <i>double IOHprofiler_uniform_rand()</i>, returns a random number from standard uniform distribution.
* <i>double IOHprofiler_normal_rand()</i>, returns a random number from standard normal distribution.

### IOHprofiler_observer

`IOHprofiler_observer` defines triggers of recording evaluations, and __logger__ classes inherit it to set up the time of recording.