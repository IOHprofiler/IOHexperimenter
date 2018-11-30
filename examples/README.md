  - `example` contains examples of using the tool.
  - `example/example1` contains the source code of random search method.
  - `example/example1` contains the source code of one plus lambda EA.
  - `example/algorithms` contains source codes of several algorithms. Please rename files as "user_algorithm.c" if you want to test.

  After replacing files in `../code-experiments/build/c/` [`../code-experiments/build/python/`] by the corresponding files in `/example/example*/c` [`/example/example*/python`],
  run the statement
  ```
  	python do.py run-c
  ```
  or
  ```
  	python do.py run-python
  ```
  in the root folder to get result.
