# -*- mode: cython -*-
#cython: c_string_type=str, c_string_encoding=ascii
from __future__ import absolute_import, division, print_function, unicode_literals
import sys
import numpy as np
cimport numpy as np

from IOHprofiler_python.exceptions import InvalidProblemException, NoSuchProblemException, NoSuchSuiteException

known_suite_names = [b"PBO"]
_known_suite_names = [b"PBO"]

# _test_assignment = "seems to prevent an 'export' error (i.e. induce export) to make this module known under Linux and Windows (possibly because of the leading underscore of _interface)"
# __all__ = ['Problem', 'Benchmark']

# Must initialize numpy or risk segfaults
np.import_array()

cdef extern from "IOHprofiler.h":
    ctypedef struct IOHprofiler_problem_t:
        pass
    ctypedef struct IOHprofiler_observer_t:
        pass
    ctypedef struct IOHprofiler_suite_t: 
        pass

    const char* IOHprofiler_set_log_level(const char *level)

    #functions for IOHprofiler_suite#
    IOHprofiler_suite_t *IOHprofiler_suite(const char *suite_name, const char *suite_instance, 
                             const char *suite_options)
    void IOHprofiler_suite_free(IOHprofiler_suite_t *suite)
    IOHprofiler_problem_t *IOHprofiler_problem_duplicate(const IOHprofiler_problem_t*)
    IOHprofiler_problem_t* IOHprofiler_suite_get_next_problem(IOHprofiler_suite_t*, IOHprofiler_observer_t*)
    IOHprofiler_problem_t *IOHprofiler_suite_reset_problem(IOHprofiler_suite_t *suite, IOHprofiler_observer_t *observer);
    IOHprofiler_problem_t* IOHprofiler_suite_get_problem(IOHprofiler_suite_t *, const size_t)
    IOHprofiler_problem_t *IOHprofiler_suite_get_problem_by_function_dimension_instance(IOHprofiler_suite_t *suite, const size_t function,
                                                                          const size_t dimension, const size_t instance)
    #end functions for IOHprofiler_suite#
    
    #functions for IOHprofiler_observer#
    IOHprofiler_observer_t *IOHprofiler_observer(const char *observer_name, const char *options)
    void IOHprofiler_observer_free(IOHprofiler_observer_t *self)
    IOHprofiler_problem_t *IOHprofiler_problem_add_observer(IOHprofiler_problem_t *problem, 
                                              IOHprofiler_observer_t *observer) 
    #end functions for IOHprofiler_observer#
   
    #functions for IOHprofiler_problem#
    void IOHprofiler_evaluate_function(IOHprofiler_problem_t *problem, const int *x, double *y)
    void IOHprofiler_evaluate_constraint(IOHprofiler_problem_t *problem, const int *x, double *y)
    void IOHprofiler_recommend_solution(IOHprofiler_problem_t *problem, const int *x)
    void IOHprofiler_problem_free(IOHprofiler_problem_t *problem)
    void IOHprofiler_problem_set_parameters(IOHprofiler_problem_t *problem, const int number_of_parameters, const double *parameters)
    const char *IOHprofiler_problem_get_name(const IOHprofiler_problem_t *problem)
    const char *IOHprofiler_problem_get_id(const IOHprofiler_problem_t *problem)
    size_t IOHprofiler_problem_get_dimension(const IOHprofiler_problem_t *problem)
    size_t IOHprofiler_problem_get_number_of_objectives(const IOHprofiler_problem_t *problem)
    size_t IOHprofiler_problem_get_evaluations(const IOHprofiler_problem_t *problem)
    int IOHprofiler_problem_final_target_hit(const IOHprofiler_problem_t *problem)
    double IOHprofiler_problem_get_best_observed_fvalue1(const IOHprofiler_problem_t *problem)
    double IOHprofiler_problem_get_final_target_fvalue1(const IOHprofiler_problem_t *problem)
    const int *IOHprofiler_problem_get_smallest_values_of_interest(const IOHprofiler_problem_t *problem)
    const int *IOHprofiler_problem_get_largest_values_of_interest(const IOHprofiler_problem_t *problem)
    size_t IOHprofiler_problem_get_suite_dep_index(const IOHprofiler_problem_t* problem)
    void IOHprofiler_problem_get_initial_solution(IOHprofiler_problem_t *problem, double *x)
    #end functions for IOHprofiler_problem#


cdef bytes _bstring(s):
    if type(s) is bytes:
        return <bytes>s
    elif isinstance(s, unicode):
        return s.encode('ascii')
    else:
        raise TypeError("expect a string, got %s" % str(type(s)))

cdef IOHprofiler_observer_t* _current_observer

cdef class Suite:

    cdef IOHprofiler_suite_t* suite  # AKA _self
    cdef IOHprofiler_problem_t* _current_problem
    cdef bytes _name  # used in @property name
    cdef bytes _instance
    cdef bytes _options
    cdef current_problem_  # name _current_problem is taken
    cdef _current_index
    cdef _ids
    cdef _indices
    cdef _names
    cdef _dimensions
    cdef _number_of_objectives
    cdef initialized

    def __cinit__(self, suite_name, suite_instance, suite_options):
        cdef np.npy_intp shape[1]  # probably completely useless
        self._name = _bstring(suite_name)
        self._instance = _bstring(suite_instance if suite_instance is not None else "")
        self._options = _bstring(suite_options if suite_options is not None else "")
        self._current_problem = NULL
        self.current_problem_ = None
        self._current_index = None
        self.initialized = False
        self._initialize()
        assert self.initialized
    cdef _initialize(self):
        """sweeps through `suite` to collect indices and id's to operate by
        direct access in the remainder"""
        cdef np.npy_intp shape[1]  # probably completely useless
        cdef IOHprofiler_suite_t* suite
        cdef IOHprofiler_problem_t* p
        cdef bytes _old_level

        if self.initialized:
            self.reset()
        self._ids = []
        self._indices = []
        self._names = []
        self._dimensions = []
        self._number_of_objectives = []
        if str(self._name) not in [str(n) for n in known_suite_names]:
            raise NoSuchSuiteException("""Unkown benchmark suite name "%s".
Known suite names are %s.
If "%s" was not a typo, you can add the desired name to `known_suite_names`::

        >> import IOHprofiler_python as ex
        >> ex.known_suite_names.append(b"my_name")  # must be a byte string
        >> suite = ex.Suite("my_name", "", "")
        IOHprofiler FATAL ERROR: IOHprofiler_suite(): unknow problem suite

This will crash Python, if the suite "my_name" does in fact not exist.
""" % (self._name, str(known_suite_names), self._name))
        try:
            suite = IOHprofiler_suite(self._name, self._instance, self._options)
        except:
            raise NoSuchSuiteException("No suite with name '%s' found" % self._name)
        if suite == NULL:
            raise NoSuchSuiteException("No suite with name '%s' found" % self._name)
        while True:
            old_level = log_level('warning')
            p = IOHprofiler_suite_get_next_problem(suite, NULL)
            log_level(old_level)
            if not p:
                break
            self._indices.append(IOHprofiler_problem_get_suite_dep_index(p))
            self._ids.append(IOHprofiler_problem_get_id(p))
            self._names.append(IOHprofiler_problem_get_name(p))
            self._dimensions.append(IOHprofiler_problem_get_dimension(p))
            self._number_of_objectives.append(IOHprofiler_problem_get_number_of_objectives(p))
        IOHprofiler_suite_free(suite)
        self.suite = IOHprofiler_suite(self._name, self._instance, self._options)
        self.initialized = True
        return self
    def reset(self):
        """reset to original state, affecting `next_problem()`,
        `current_problem`, `current_index`"""
        self._current_index = None
        if self.current_problem_:
            self.current_problem_.free()
        self.current_problem_ = None
        self._current_problem = NULL
    def reset_problem(self, observer=None):
        """`next_problem(observer=None)` returns the "next" problem in the
        `Suite`, on the first call or after `reset()` the first problem.

        `next_problem` serves to sweep through the `Suite` smoothly.
        """
        cdef size_t index
        global _current_observer
        if not self.initialized:
            raise ValueError("Suite has been finalized/free'ed")
        if self.current_problem_:
            self.current_problem_.free()
        if self._current_index is None:
            self._current_index = 0
        if self._current_index >= len(self):
            self._current_problem = NULL
            self.current_problem_ = None
            # self._current_index = -1  # or use reset?
        else:
            index = self.indices[self._current_index]  # "conversion" to size_t
            self._current_problem = IOHprofiler_suite_get_problem(
                                        self.suite, index)
            self.current_problem_ = Problem_init(self._current_problem,
                                                True, self._name)
            self.current_problem_.observe_with(observer)
        return self.current_problem_
    def next_problem(self, observer=None):
        """`next_problem(observer=None)` returns the "next" problem in the
        `Suite`, on the first call or after `reset()` the first problem.

        `next_problem` serves to sweep through the `Suite` smoothly.
        """
        cdef size_t index
        global _current_observer
        if not self.initialized:
            raise ValueError("Suite has been finalized/free'ed")
        if self.current_problem_:
            self.current_problem_.free()
        if self._current_index is None:
            self._current_index = -1
        self._current_index += 1
        if self._current_index >= len(self):
            self._current_problem = NULL
            self.current_problem_ = None
            # self._current_index = -1  # or use reset?
        else:
            index = self.indices[self._current_index]  # "conversion" to size_t
            self._current_problem = IOHprofiler_suite_get_problem(
                                        self.suite, index)
            self.current_problem_ = Problem_init(self._current_problem,
                                                True, self._name)
            self.current_problem_.observe_with(observer)
        return self.current_problem_
    def get_problem(self, id, observer=None):
        """`get_problem(self, id, observer=None)` returns a `Problem` instance,
        by default unobserved, using `id: str` or index (where `id: int`) to
        identify the desired problem.

        All values between zero and `len(self) - 1` are valid index values::

        >>> import IOHprofilerex as ex
        >>> suite = ex.Suite("PBO", "", "")
        >>> for index in range(len(suite)):
        ...     problem = suite.get_problem(index)
        ...     # work work work using problem
        ...     problem.free()

        A shortcut for `suite.get_problem(index)` is `suite[index]`, they are
        synonym.

        Details:
        - Here an `index` takes values between 0 and `len(self) - 1` and can in
          principle be different from the problem index in the benchmark suite.

        - This call does not affect the state of the `current_problem` and
          `current_index` attributes.
        """
        if not self.initialized:
            raise ValueError("Suite has been finalized/free'ed")
        index = id
        try:
            1 / (id == int(id))  # int(id) might raise an exception
        except:
            index = self._ids.index(id)
        try:
            return Problem_init(IOHprofiler_suite_get_problem(self.suite, self._indices[index]),
                                True, self._name).observe_with(observer)
        except:
            raise NoSuchProblemException(self.name, str(id))

    def get_problem_by_function_dimension_instance(self, function, dimension, instance, observer=None):
        """returns a `Problem` instance, by default unobserved, using function,
        dimension and instance to identify the desired problem.

        If a suite contains multiple problems with the same function, dimension
        and instance, the first corresponding problem is returned.

        >>> import IOHprofiler_python as ex
        >>> suite = ex.Suite("PBO", "", "")
        >>> problem = suite.get_problem_by_function_dimension_instance(1, 2, 3)
        >>> # work using problem
        >>> problem.free()

        Details:
        - Function, dimension and instance are integer values from 1 on.

        - This call does not affect the state of the `current_problem` and
          `current_index` attributes.

        - For some suites and/or observers, the `free()` method of the problem
          must be called before the next call of
          `get_problem_by_function_dimension_instance`. Otherwise Python might
          just silently die, which is e.g. a known issue of the "PBO" observer.
        """
        cdef size_t _function = function # "conversion" to size_t
        cdef size_t _dimension = dimension # "conversion" to size_t
        cdef size_t _instance = instance # "conversion" to size_t

        if not self.initialized:
            raise ValueError("Suite has been finalized/free'ed")
        try:
            return Problem_init(IOHprofiler_suite_get_problem_by_function_dimension_instance(self.suite, _function,
                                                                                      _dimension, _instance),
                                True, self._name).observe_with(observer)
        except:
            raise NoSuchProblemException(self.name, 'function: {}, dimension: {}, instance: {}'.format(function,
                                                                                                       dimension,
                                                                                                       instance))

    def __getitem__(self, key):
        """`self[i]` is a synonym for `self.get_problem(i)`, see `get_problem`
        """
        return self.get_problem(key)

    def free(self):
        """free underlying C structures"""
        self.__dealloc__()
        self.suite = NULL
        self.initialized = False  # not (yet) visible from outside
    def __dealloc__(self):
        if self.suite:
            IOHprofiler_suite_free(self.suite)

    def find_problem_ids(self, *args, **kwargs):
        """has been renamed to `ids`"""
        raise NotImplementedError(
            "`find_problem_ids()` has been renamed to `ids()`")


    def ids(self, *id_snippets, get_problem=False, verbose=False):

        res = []
        for idx, id in enumerate(self._ids):
            if all([id.find(i) >= 0 for i in id_snippets]):
                if verbose:
                    print("  id=%s, index=%d" % (id, idx))
                res.append(id)
        if get_problem:
            return self.get_problem(res[0])
        return res

    @property
    def current_problem(self):
        """current "open/active" problem to be benchmarked"""
        return self.current_problem_
    @property
    def current_index(self):
        return self._current_index
    @property
    def problem_names(self):
        """list of problem names in this `Suite`, see also `ids`"""
        return list(self._names)
    @property
    def dimensions(self):
        """list of problem dimensions occuring at least once in this `Suite`"""
        return sorted(set(self._dimensions))
    @property
    def number_of_objectives(self):
        """list of number of objectives occuring in this `Suite`"""
        return sorted(set(self._number_of_objectives))
    @property
    def indices(self):
        """list of all problem indices, deprecated.
        
        These values are (only) used to call the underlying C structures.
        Indices used in the Python interface run between 0 and `len(self)`.
        """
        return list(self._indices)
    @property
    def name(self):
        """name of this suite as used to instantiate the suite via `Suite(name, ...)`"""
        return self._name
    @property
    def instance(self):
        """instance of this suite as used to instantiate the suite via
        `Suite(name, instance, ...)`"""
        return self._instance
    @property
    def options(self):
        """options for this suite as used to instantiate the suite via
        `Suite(name, instance, options)`"""
        return self._options

    @property
    def info(self):
        return str(self)
    def __repr__(self):
        return 'Suite(%r, %r, %r)'  % (self.name, self.instance, self.options)  # angled brackets
    def __str__(self):
        return 'Suite("%s", "%s", "%s") with %d problem%s in dimension%s %s' \
            % (self.name, self.instance, self.options,
               len(self), '' if len(self) == 1 else 's',
               '' if len(self.dimensions) == 1 else 's',
               '%d=%d' % (min(self.dimensions), max(self.dimensions)))
    def __len__(self):
        return len(self._indices)

    def __iter__(self):
        """iterator over self.

        CAVEAT: this function uses `next_problem` and has a side effect on the
        state of the `current_problem` and `current_index` attributes. `reset()`
        rewinds the suite to the initial state. """
        if 1 < 3:
            s = self
            s.reset()
        else:
            s = Suite(self.name, self.instance, self.options)
        try:
            while True:
                try:
                    problem = s.next_problem()
                    if problem is None:
                        raise StopIteration
                except NoSuchProblemException:
                    raise StopIteration
                yield problem
        except:
            raise
        finally:  # makes this ctrl-c safe, at least it should
            s is self or s.free()

cdef class Observer:
    """Observer which can be "attached to" one or several problems, however not
    necessarily at the same time.

    The typical observer records data to be used in the post-processing
    module afterwards.

    >>> import IOHprofiler_python as ex
    >>> suite = ex.Suite("PBO", "", "")
    >>> f = suite.get_problem(1)
    >>> observer = ex.Observer("IOHprofiler",
    ...                        "result_folder: doctest")
    >>> f.observe_with(observer)  # the same as observer.observe(f)  # doctest: +ELLIPSIS
    <IOHprofilerex...
    >>> # work work work with observed f
    >>> f.free()

    """
    cdef IOHprofiler_observer_t* _observer
    cdef bytes _name
    cdef bytes _options
    cdef _state

    def __cinit__(self, name, options):
        if isinstance(options, dict):
            s = str(options).replace(',', ' ')
            for c in ["u'", 'u"', "'", '"', "{", "}"]:
                s = s.replace(c, '')
            options = s
        self._name = _bstring(name)
        self._options = _bstring(options if options is not None else "")
        self._observer = IOHprofiler_observer(self._name, self._options)
        self._state = 'initialized'

    def _update_current_observer_global(self):
        """assign the global _current_observer variable to self._observer, 
        for purely technical reasons"""
        global _current_observer
        _current_observer = self._observer

    def observe(self, problem):
        """`observe(problem)` let `self` observe the `problem: Problem` by
        calling `problem.observe_with(self)`.
        """
        problem.observe_with(self)
        return self

    @property
    def name(self):
        """name of the observer as used with `Observer(name, ...)` to instantiate
        `self` before.
        """
        return self._name
    @property
    def options(self):
        return self._options
    @property
    def state(self):
        return self._state

    def free(self):
        self.__dealloc__()
        self._observer = NULL
        self._state = 'deactivated'
    def __dealloc__(self):
        if self._observer !=  NULL:
            IOHprofiler_observer_free(self._observer)

cdef Problem_init(IOHprofiler_problem_t* problem, free=True, suite_name=None):
    """`Problem` class instance initialization wrapper passing 
    a `problem_t*` C-variable to `__init__`. 
    
    This is necessary because __cinit__ cannot be defined as cdef, only as def. 
    """
    res = Problem()
    res._suite_name = suite_name
    return res._initialize(problem, free)
cdef class Problem:
    """`Problem` instances are usually generated using `Suite`.
    
    The main feature of a problem instance is that it is callable, returning the
    objective function value when called with a candidate solution as input.
    """
    cdef IOHprofiler_problem_t* problem
    cdef np.ndarray y_values  # argument for IOHprofiler_evaluate
    cdef np.ndarray x_initial  # argument for IOHprofiler_problem_get_initial_solution
    cdef np.ndarray parameters # argument for IOHprofiler_problem_set_parameters
    cdef public np.ndarray _lower_bounds
    cdef public np.ndarray _upper_bounds
    cdef size_t _number_of_variables
    cdef size_t _number_of_objectives
    cdef size_t _number_of_parameters
    cdef _suite_name  # for the record
    cdef _list_of_observers  # for the record
    cdef _problem_index  # for the record, this is not public but used in index property
    cdef _do_free
    cdef initialized
    def __cinit__(self):
        cdef np.npy_intp shape[1]
        self.initialized = False  # all done in _initialize
    cdef _initialize(self, IOHprofiler_problem_t* problem, free=True):
        cdef np.npy_intp shape[1]
        if self.initialized:
            raise RuntimeError("Problem already initialized")
        if problem == NULL:
            raise ValueError("in Problem._initialize(problem,...): problem is NULL")
        self.problem = problem
        self._problem_index = IOHprofiler_problem_get_suite_dep_index(self.problem)
        self._do_free = free
        self._list_of_observers = []
    
        self._number_of_variables = IOHprofiler_problem_get_dimension(self.problem)
        self._number_of_objectives = IOHprofiler_problem_get_number_of_objectives(self.problem)
        self.y_values = np.zeros(self._number_of_objectives)
        self.x_initial = np.zeros(self._number_of_variables,dtype = int)
        ## FIXME: Inefficient because we copy the bounds instead of
        ## sharing the data.
        self._lower_bounds = -np.inf * np.ones(self._number_of_variables)
        self._upper_bounds = np.inf * np.ones(self._number_of_variables)
        for i in range(self._number_of_variables):
            if IOHprofiler_problem_get_smallest_values_of_interest(self.problem) is not NULL:
                self._lower_bounds[i] = IOHprofiler_problem_get_smallest_values_of_interest(self.problem)[i]
            if IOHprofiler_problem_get_largest_values_of_interest(self.problem) is not NULL:
                self._upper_bounds[i] = IOHprofiler_problem_get_largest_values_of_interest(self.problem)[i]
        self.initialized = True
        return self
    def recommend(self, arx):
        """Recommend a solution, return `None`.

        The recommendation replaces the last evaluation or recommendation
        for the assessment of the algorithm.
        """
        raise NotImplementedError("has never been tested, incomment this to start testing")
        cdef np.ndarray[double, ndim=1, mode="c"] _x
        x = np.array(x, copy=False, dtype=np.double, order='C')
        if np.size(x) != self.number_of_variables:
            raise ValueError(
                "Dimension, `np.size(x)==%d`, of input `x` does " % np.size(x) +
                "not match the problem dimension `number_of_variables==%d`."
                % self.number_of_variables)
        _x = x  # this is the final type conversion
        if self.problem is NULL:
            raise InvalidProblemException()
        IOHprofiler_recommend_solution(self.problem, <double *>np.PyArray_DATA(_x))

    def add_observer(self, observer):
        """`add_observer(self, observer: Observer)`, see `observe_with`.
        """
        return self.observe_with(observer)

    def observe_with(self, observer):
        """`observe_with(self, observer: Observer)` attaches an observer
        to this problem.

        Attaching an observer can be considered as wrapping the observer
        around the problem. For the observer to be finalized, the problem
        must be free'd (implictly or explicitly).

        Details: `observer` can be `None`, in which case nothing is done.

        See also: class `Observer`
        """
        if observer:
            assert self.problem
            observer._update_current_observer_global()
            self.problem = IOHprofiler_problem_add_observer(self.problem, _current_observer)
            self._list_of_observers.append(observer)
        return self

    def _f0(self, x):
        """"inofficial" interface to `self` with target f-value of zero. """
        return self(x) - self.final_target_fvalue1

    def set_parameters(self, parameters):
        """`set_parameters(self, parameters: array)` set parameters of  
        algorithms/optimizers for the record.
        """
        self._number_of_parameters = len(parameters)
        self.parameters = parameters
        IOHprofiler_problem_set_parameters(self.problem,self._number_of_parameters,<double *>np.PyArray_DATA(self.parameters))
    @property
    def initial_solution(self):
        """return feasible initial solution"""
        IOHprofiler_problem_get_initial_solution(self.problem,
                                          <double *>np.PyArray_DATA(self.x_initial))
        return np.array(self.x_initial, copy=True)
    @property
    def list_of_observers(self):
        return self._list_of_observers
    property number_of_variables:  # this is cython syntax, not known in Python
        # this is a class definition which is instantiated automatically!?
        """Number of variables this problem instance expects as input."""
        def __get__(self):
            return self._number_of_variables
    @property
    def dimension(self):
        """alias for `number_of_variables` of the input space"""
        return self._number_of_variables
    @property
    def number_of_objectives(self):
        "number of objectives, if equal to 1, call returns a scalar"
        return self._number_of_objectives
    @property
    def number_of_constraints(self):
        "number of constraints"
        return self._number_of_constraints
    @property
    def lower_bounds(self):
        """depending on the test bed, these are not necessarily strict bounds
        """
        return self._lower_bounds
    @property
    def upper_bounds(self):
        """depending on the test bed, these are not necessarily strict bounds
        """
        return self._upper_bounds
    @property
    def evaluations(self):
        return IOHprofiler_problem_get_evaluations(self.problem)
    @property
    def final_target_hit(self):
        """return 1 if the final target is known and has been hit, 0 otherwise
        """
        assert(self.problem)
        return IOHprofiler_problem_final_target_hit(self.problem)
    @property
    def final_target_fvalue1(self):
        assert(self.problem)
        return IOHprofiler_problem_get_final_target_fvalue1(self.problem)
    @property
    def best_observed_fvalue1(self):
        assert(self.problem)
        return IOHprofiler_problem_get_best_observed_fvalue1(self.problem)

    def free(self, force=False):
        """Free the given test problem.

        Not strictly necessary (unless, possibly, for the observer). `free`
        ensures that all files associated with the problem are closed as
        soon as possible and any memory is freed. After free()ing the
        problem, all other operations are invalid and will raise an
        exception.
        """
        if self.problem != NULL and (self._do_free or force):
            IOHprofiler_problem_free(self.problem)
            self.problem = NULL

    def __dealloc__(self):
        # see http://docs.cython.org/src/userguide/special_methods.html
        # free let the problem_free() call(s) in IOHprofiler_suite_t crash, hence
        # the possibility to set _do_free = False
        if self._do_free and self.problem != NULL:  # this is not guaranteed to work, see above link
            IOHprofiler_problem_free(self.problem)

    # def __call__(self, np.ndarray[double, ndim=1, mode="c"] x):
    def __call__(self, x):
        """return objective function value of input `x`"""
        cdef np.ndarray[int, ndim=1, mode="c"] _x
        assert self.initialized
        x = np.array(x, copy=False, dtype=np.int32, order='C')
        if np.size(x) != self.number_of_variables:
            raise ValueError(
                "Dimension, `np.size(x)==%d`, of input `x` does " % np.size(x) +
                "not match the problem dimension `number_of_variables==%d`." 
                             % self.number_of_variables)
        _x = x  # this is the final type conversion
        if self.problem is NULL:
            raise InvalidProblemException()
        IOHprofiler_evaluate_function(self.problem,
                               <int *>np.PyArray_DATA(_x),
                               <double *>np.PyArray_DATA(self.y_values))
        if self._number_of_objectives == 1:
            return self.y_values[0]
        return np.array(self.y_values, copy=True)

    @property
    def id(self):
        "id as string without spaces or weird characters"
        if self.problem is not NULL:
            return IOHprofiler_problem_get_id(self.problem)

    @property
    def name(self):
        if self.problem is not NULL:
            return IOHprofiler_problem_get_name(self.problem)

    @property
    def index(self):
        """problem index in the benchmark `Suite` of origin"""
        return self._problem_index

    @property
    def suite(self):
        """benchmark suite this problem is from"""
        return self._suite_name

    @property
    def info(self):
        return str(self)

    def __str__(self):
        if self.problem is not NULL:
            objective = "%s-objective" % ('single'
                    if self.number_of_objectives == 1 
                    else str(self.number_of_objectives))
            return "%s %s problem (%s)" % (self.id, objective,  
                self.name.replace(self.name.split()[0], 
                                  self.name.split()[0] + "(%d)" 
                                  % (self.index if self.index is not None else -2)))
        else:
            return "finalized/invalid problem"
    
    def __repr__(self):
        if self.problem is not NULL:
            return "<%s(), id=%r>" % (
                    repr(self.__class__).split()[1][1:-2],
                    # self.problem_suite, self.problem_index,
                    self.id)
        else:
            return "<finalized/invalid problem>"
        
    def __enter__(self):
        """Allows ``with Benchmark(...).get_problem(...) as problem:``"""
        return self
    def __exit__(self, exception_type, exception_value, traceback):
        try:
            self.free()
        except:
            pass

def log_level(level=None):
    """`log_level(level=None)` return current log level and
    set new log level if `level is not None and level`.
    
    `level` must be 'error' or 'warning' or 'info' or 'debug', listed
    with increasing verbosity, or '' which doesn't change anything.
    """
    cdef bytes _level = _bstring(level if level is not None else "")
    return IOHprofiler_set_log_level(_level)

