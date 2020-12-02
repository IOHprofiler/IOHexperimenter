
# Python Interface to IOHexperimenter

This is the python interface to [__IOHexperimenter__](https://github.com/IOHprofiler/IOHexperimenter), which is the experimentation environment for Iterative Optimization Heuristics (IOHs). The source code of IOHexperimenter is written in _C++_ and its python interface is realized using [__SWIG__](http://www.swig.org/). IOHexperimenter is a part of the bigger project called [__IOHprofiler__](https://iohprofiler.github.io/).

## build/upload the python package

To build the source file:

```python
python setup.py build
```

To install the built package locally:

```python
python setup.py install
```

To prepare the source distribution package:

```python
python setup.py sdist
```

To distribute package to PyPI:

```python
python -m twine upload dist/*
```

Of course, you need install `twine` package first.
