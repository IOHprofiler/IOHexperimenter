.. IOHExperimenter documentation master file, created by
   sphinx-quickstart on Tue Jan 18 14:10:42 2022.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

IOHexperimenter
===========================================

This page contains the full documentation for the python-interface of the IOHexperimenter.
Note that a separate getting-started guide is available on our wiki: https://iohprofiler.github.io/IOHexp/python/


The python-version of IOHexperimenter is built using pybind11, and desiged so most functions closely match their C++ equivalents, with a few added ease-of-use settings.
The IOHexperimenter documentation is split into four sections (available in the sidebar):
* ioh, which provides definitions of the python-specific 'get_problem' function, and the help-classes used throughout the package.
* problem, which provide the raw objective functions themselves and handle the related concepts such as search-space transformations.
* logger, which deals with both the actual writing of logging data, as well as deciding when these logging events should be triggered
* suite, which is a way of collecting multiple functions together

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
