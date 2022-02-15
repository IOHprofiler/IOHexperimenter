Python
===============

This page contains the full documentation for the python-interface of the IOHexperimenter.

The python-version of IOHexperimenter is built using pybind11, and desiged so most functions closely match their C++ equivalents, with a few added ease-of-use settings.
The IOHexperimenter documentation is split into four sections (available in the sidebar):

* ioh, which provides definitions of the python-specific 'get_problem' function, and the help-classes used throughout the package.
* problem, which provide the raw objective functions themselves and handle the related concepts such as search-space transformations.
* logger, which deals with both the actual writing of logging data, as well as deciding when these logging events should be triggered
* suite, which is a way of collecting multiple functions together

.. mdinclude:: ../../../ioh/README.md


API documentation
-----------------

.. toctree::
   :maxdepth: 10
   
   python/ioh.rst
   python/structures.rst
   python/problem.rst
   python/logger.rst
   python/suite.rst 





