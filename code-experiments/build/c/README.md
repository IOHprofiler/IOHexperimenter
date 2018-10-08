Using IOHprofiler by C 
==============================================

Getting started
---------------

See [here](../../../README.md#Getting-Started) for the first steps and execute the 
```
python do.py build-c
``` 
Then,
- Copy the files `user_experiment.c`, `user_algorithm.c`, `configuration.ini`, `IOHprofiler.c`, `IOHprofiler.h` and `Makefile` to a folder of your choice. Modify the `user_experiment.c` and `configuration.ini` files to include your optimization algorithm.

- Invoke `make` to compile and run your experiment.

Easy steps for using the IOHprofiler
---------------
- `step 1` replace the content of user_algorithm by your algorithm.
- `step 2` modify the configuration file to collect problems.
- `step 3` run the statement 
```
python do.py run-c
``` 
in the root folder, or `make` in the folder where copies files are located at.