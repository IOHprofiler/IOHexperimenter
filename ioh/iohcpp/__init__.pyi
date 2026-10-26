"""
Python Interface to IOHexperimenter, which provides

    * A framework for straightforward benchmarking of any iterative optimization heuristic
    * A suite consisting of 23 pre-made Pseudo-Boolean benchmarking function, with easily
       accessible methods for adding custom functions and suites
    * Logging methods to effortlesly store benchmarking data in a format compatible with
      IOHanalyzer, with future support for additional data logging options
It is one part of the IOHprofiler project (https://github.com/IOHprofiler)

Examples
--------
```
```

Contact us
----------
If you have any questions, comments or suggestions, please don't hesitate contacting
us via IOHprofiler@liacs.leidenuniv.nl

Cite us
-------
@ARTICLE{IOHprofiler,
    author = {Carola Doerr and Hao Wang and Furong Ye and Sander van Rijn and Thomas B{\\"a}ck},
    title = {{IOHprofiler: A Benchmarking and Profiling Tool for Iterative Optimization Heuristics}},
    journal = {arXiv e-prints:1810.05281},
    archivePrefix = "arXiv",
    eprint = {1810.05281},
    year = 2018,
    month = oct,
    keywords = {Computer Science - Neural and Evolutionary Computing},
    url = {https://arxiv.org/abs/1810.05281}
}
"""

from collections.abc import Sequence
import enum
from typing import Annotated, overload

import numpy
from numpy.typing import NDArray

from ioh.iohcpp import (
    logger as logger,
    problem as problem,
    suite as suite
)


class DataError(RuntimeError):
    pass

def set_data_root(path: str) -> None:
    """
    Sets the directory of the data files (the static folder of IOHexperimenter); an empty path unsets it.
    """

def data_root() -> str:
    """
    The directory of the data files: the one set with set_data_root, otherwise IOH_RESOURCES.
    """

class OptimizationType(enum.Enum):
    """
    Enum used for defining whether the problem is subject to minimization or maximization
    """

    MAX = 1

    MIN = 0

    def __int__(self) -> int: ...

    def __index__(self) -> int: ...

MAX: OptimizationType = OptimizationType.MAX

MIN: OptimizationType = OptimizationType.MIN

class ConstraintEnforcement(enum.Enum):
    """Enum defining constraint handling strategies."""

    NOT = 0
    """Do not calculate the constraint at all"""

    HIDDEN = 1
    """Do not enforce, but still calculate to enable logging."""

    SOFT = 2
    """
    Penalize (y + p), but aggregate all the constraint penalties into a sum
    """

    HARD = 3
    """
    Penalize (p only), and if violation return only the penalty for this constraint in contraintset
    """

    OVERRIDE = 4
    """
    Penalize (p only), and if violation return the custom penalization function this constraint in contraintset
    """

    def __int__(self) -> int: ...

    def __index__(self) -> int: ...

NOT: ConstraintEnforcement = ConstraintEnforcement.NOT

HIDDEN: ConstraintEnforcement = ConstraintEnforcement.HIDDEN

SOFT: ConstraintEnforcement = ConstraintEnforcement.SOFT

HARD: ConstraintEnforcement = ConstraintEnforcement.HARD

OVERRIDE: ConstraintEnforcement = ConstraintEnforcement.OVERRIDE

class RealSolution:
    @overload
    def __init__(self, x: Annotated[NDArray[numpy.float64], dict(shape=(None,), order='C', device='cpu', writable=False)], y: float) -> None: ...

    @overload
    def __init__(self, x: Sequence[float], y: float) -> None:
        """
        A Solution object, which is a container for a search point and its corresponding fitness value.

        Parameters
        ----------
        x: list
            the search point in a search space, e.g., R^d or {0,1}^d
        y: float
            the corresponding objective value of `x`, i.e., y = f(x)
        """

    @property
    def x(self) -> Annotated[NDArray[numpy.float64], dict(shape=(None,))]:
        """The search point in a search space, e.g., R^d or {0,1}^d"""

    @x.setter
    def x(self, arg: Sequence[float], /) -> None: ...

    @property
    def y(self) -> float:
        """The corresponding objective value of `x`, i.e., y = f(x)"""

    def __repr__(self) -> str: ...

class IntegerSolution:
    @overload
    def __init__(self, x: Annotated[NDArray[numpy.int64], dict(shape=(None,), order='C', device='cpu', writable=False)], y: float) -> None: ...

    @overload
    def __init__(self, x: Sequence[int], y: float) -> None:
        """
        A Solution object, which is a container for a search point and its corresponding fitness value.

        Parameters
        ----------
        x: list
            the search point in a search space, e.g., R^d or {0,1}^d
        y: float
            the corresponding objective value of `x`, i.e., y = f(x)
        """

    @property
    def x(self) -> Annotated[NDArray[numpy.int32], dict(shape=(None,))]:
        """The search point in a search space, e.g., R^d or {0,1}^d"""

    @x.setter
    def x(self, arg: Sequence[int], /) -> None: ...

    @property
    def y(self) -> float:
        """The corresponding objective value of `x`, i.e., y = f(x)"""

    def __repr__(self) -> str: ...

class AbstractIntegerConstraint:
    @property
    def enforced(self) -> ConstraintEnforcement:
        """The type of constraint enforcement applied."""

    @enforced.setter
    def enforced(self, arg: ConstraintEnforcement, /) -> None: ...

    @property
    def weight(self) -> float:
        """The weight given to this constraint"""

    @weight.setter
    def weight(self, arg: float, /) -> None: ...

    @property
    def exponent(self) -> float:
        """The exponent for this constraint"""

    @exponent.setter
    def exponent(self, arg: float, /) -> None: ...

    @overload
    def __call__(self, arg0: Annotated[NDArray[numpy.int64], dict(shape=(None,), order='C', device='cpu', writable=False)], arg1: float, /) -> float: ...

    @overload
    def __call__(self, arg0: Sequence[int], arg1: float, /) -> float: ...

    @overload
    def is_feasible(self, arg: Annotated[NDArray[numpy.int64], dict(shape=(None,), order='C', device='cpu', writable=False)], /) -> bool: ...

    @overload
    def is_feasible(self, arg: Sequence[int], /) -> bool: ...

    @overload
    def compute_violation(self, arg: Annotated[NDArray[numpy.int64], dict(shape=(None,), order='C', device='cpu', writable=False)], /) -> bool: ...

    @overload
    def compute_violation(self, arg: Sequence[int], /) -> bool: ...

    def penalize(self, arg: float, /) -> float: ...

    def penalty(self) -> float: ...

    def violation(self) -> float: ...

class IntegerConstraintSet:
    def __init__(self, constraints: Sequence[AbstractIntegerConstraint] = []) -> None:
        """
        A set of constraints

        Parameters
        ----------
            constraints: list[Constraint]
                The list of constraints
        """

    def add(self, arg: AbstractIntegerConstraint, /) -> None: ...

    def remove(self, arg: AbstractIntegerConstraint, /) -> None: ...

    def penalize(self, arg: float, /) -> float: ...

    def penalty(self) -> float: ...

    def violation(self) -> float: ...

    def n(self) -> int: ...

    def __getitem__(self, arg: int, /) -> AbstractIntegerConstraint: ...

    def __repr__(self) -> str: ...

class IntegerConstraint(AbstractIntegerConstraint):
    def __init__(self, fn: object, weight: float = 1.0, exponent: float = 1.0, enforced: ConstraintEnforcement = ConstraintEnforcement.SOFT, name: str = '') -> None:
        """
        General Constraint, defined by a function

        Parameters
        ----------
            f: list -> float
                A function that computes the violation of a point
            weight: float
                The weight given to the violation when penalizing the objective function
            exponent: float
                The exponent given to the violation when penalizing the objective function
            enforced: ConstraintEnforcement
                how the constraint should be enforced
            name: str
                name of the constraint
        """

    def __repr__(self) -> str: ...

class IntegerBounds(AbstractIntegerConstraint):
    @overload
    def __init__(self, lb: Annotated[NDArray[numpy.int64], dict(shape=(None,), order='C', device='cpu', writable=False)], ub: Annotated[NDArray[numpy.int64], dict(shape=(None,), order='C', device='cpu', writable=False)], enforced: ConstraintEnforcement = ConstraintEnforcement.NOT) -> None: ...

    @overload
    def __init__(self, lb: Sequence[int], ub: Sequence[int], enforced: ConstraintEnforcement = ConstraintEnforcement.NOT) -> None:
        """
        Problem Bounds, defined as a Box Constraint

        Parameters
        ----------
            lb: list
                the lower bound of the search space/domain
            ub: list
                the upper bound of the search space/domain
            enforced: ConstraintEnforcement
                whether the constraint should be enforced
        """

    @overload
    def __init__(self, size: int, lb: int, ub: int, enforced: ConstraintEnforcement = ConstraintEnforcement.NOT) -> None:
        """
        Problem Bounds, defined as a Box Constraint

        Parameters
        ----------
            size: int
                The size of the problem
            lb: float
                the lower bound of the search space/domain
            ub: float
                the upper bound of the search space/domain
            enforced: ConstraintEnforcement
                whether the constraint should be enforced
        """

    @property
    def ub(self) -> Annotated[NDArray[numpy.int32], dict(shape=(None,))]:
        """The upper bound (box constraint)"""

    @ub.setter
    def ub(self, arg: Sequence[int], /) -> None: ...

    @property
    def lb(self) -> Annotated[NDArray[numpy.int32], dict(shape=(None,))]:
        """The lower bound (box constraint)"""

    @lb.setter
    def lb(self, arg: Sequence[int], /) -> None: ...

    def __repr__(self) -> str: ...

    @overload
    def compute_violation(self, arg: Annotated[NDArray[numpy.int64], dict(shape=(None,), order='C', device='cpu', writable=False)], /) -> bool: ...

    @overload
    def compute_violation(self, arg: Sequence[int], /) -> bool:
        """
        Check if a point is within the bounds or not.

        Parameters
        ----------
            x: list | np.ndarray
                the search point to check
            y: float
                the objective value
        Returns
        -------
        bool
            Whether there is a constraint violation
        """

class AbstractRealConstraint:
    @property
    def enforced(self) -> ConstraintEnforcement:
        """The type of constraint enforcement applied."""

    @enforced.setter
    def enforced(self, arg: ConstraintEnforcement, /) -> None: ...

    @property
    def weight(self) -> float:
        """The weight given to this constraint"""

    @weight.setter
    def weight(self, arg: float, /) -> None: ...

    @property
    def exponent(self) -> float:
        """The exponent for this constraint"""

    @exponent.setter
    def exponent(self, arg: float, /) -> None: ...

    @overload
    def __call__(self, arg0: Annotated[NDArray[numpy.float64], dict(shape=(None,), order='C', device='cpu', writable=False)], arg1: float, /) -> float: ...

    @overload
    def __call__(self, arg0: Sequence[float], arg1: float, /) -> float: ...

    @overload
    def is_feasible(self, arg: Annotated[NDArray[numpy.float64], dict(shape=(None,), order='C', device='cpu', writable=False)], /) -> bool: ...

    @overload
    def is_feasible(self, arg: Sequence[float], /) -> bool: ...

    @overload
    def compute_violation(self, arg: Annotated[NDArray[numpy.float64], dict(shape=(None,), order='C', device='cpu', writable=False)], /) -> bool: ...

    @overload
    def compute_violation(self, arg: Sequence[float], /) -> bool: ...

    def penalize(self, arg: float, /) -> float: ...

    def penalty(self) -> float: ...

    def violation(self) -> float: ...

class RealConstraintSet:
    def __init__(self, constraints: Sequence[AbstractRealConstraint] = []) -> None:
        """
        A set of constraints

        Parameters
        ----------
            constraints: list[Constraint]
                The list of constraints
        """

    def add(self, arg: AbstractRealConstraint, /) -> None: ...

    def remove(self, arg: AbstractRealConstraint, /) -> None: ...

    def penalize(self, arg: float, /) -> float: ...

    def penalty(self) -> float: ...

    def violation(self) -> float: ...

    def n(self) -> int: ...

    def __getitem__(self, arg: int, /) -> AbstractRealConstraint: ...

    def __repr__(self) -> str: ...

class RealConstraint(AbstractRealConstraint):
    def __init__(self, fn: object, weight: float = 1.0, exponent: float = 1.0, enforced: ConstraintEnforcement = ConstraintEnforcement.SOFT, name: str = '') -> None:
        """
        General Constraint, defined by a function

        Parameters
        ----------
            f: list -> float
                A function that computes the violation of a point
            weight: float
                The weight given to the violation when penalizing the objective function
            exponent: float
                The exponent given to the violation when penalizing the objective function
            enforced: ConstraintEnforcement
                how the constraint should be enforced
            name: str
                name of the constraint
        """

    def __repr__(self) -> str: ...

class RealBounds(AbstractRealConstraint):
    @overload
    def __init__(self, lb: Annotated[NDArray[numpy.float64], dict(shape=(None,), order='C', device='cpu', writable=False)], ub: Annotated[NDArray[numpy.float64], dict(shape=(None,), order='C', device='cpu', writable=False)], enforced: ConstraintEnforcement = ConstraintEnforcement.NOT) -> None: ...

    @overload
    def __init__(self, lb: Sequence[float], ub: Sequence[float], enforced: ConstraintEnforcement = ConstraintEnforcement.NOT) -> None:
        """
        Problem Bounds, defined as a Box Constraint

        Parameters
        ----------
            lb: list
                the lower bound of the search space/domain
            ub: list
                the upper bound of the search space/domain
            enforced: ConstraintEnforcement
                whether the constraint should be enforced
        """

    @overload
    def __init__(self, size: int, lb: float, ub: float, enforced: ConstraintEnforcement = ConstraintEnforcement.NOT) -> None:
        """
        Problem Bounds, defined as a Box Constraint

        Parameters
        ----------
            size: int
                The size of the problem
            lb: float
                the lower bound of the search space/domain
            ub: float
                the upper bound of the search space/domain
            enforced: ConstraintEnforcement
                whether the constraint should be enforced
        """

    @property
    def ub(self) -> Annotated[NDArray[numpy.float64], dict(shape=(None,))]:
        """The upper bound (box constraint)"""

    @ub.setter
    def ub(self, arg: Sequence[float], /) -> None: ...

    @property
    def lb(self) -> Annotated[NDArray[numpy.float64], dict(shape=(None,))]:
        """The lower bound (box constraint)"""

    @lb.setter
    def lb(self, arg: Sequence[float], /) -> None: ...

    def __repr__(self) -> str: ...

    @overload
    def compute_violation(self, arg: Annotated[NDArray[numpy.float64], dict(shape=(None,), order='C', device='cpu', writable=False)], /) -> bool: ...

    @overload
    def compute_violation(self, arg: Sequence[float], /) -> bool:
        """
        Check if a point is within the bounds or not.

        Parameters
        ----------
            x: list | np.ndarray
                the search point to check
            y: float
                the objective value
        Returns
        -------
        bool
            Whether there is a constraint violation
        """

class RealState:
    def __init__(self, initial: RealSolution) -> None:
        """
        A state of a problem. Contains metrics such as the number of evaluations, and the current best value.

        Parameters
        ----------
        initial: Solution
            an object to initialize the state, which contains a placeholder for a solution to an optimization problem.
        """

    @property
    def evaluations(self) -> int:
        """The number of times the problem has been evaluated so far."""

    @property
    def optimum_found(self) -> bool:
        """
        Boolean value indicating whether final_target_found optimum of a given problem has been found.
        """

    @property
    def final_target_found(self) -> bool:
        """
        Boolean value indicating whether the final target of a given problem has been found.
        """

    @property
    def current_best_internal(self) -> RealSolution:
        """
        The internal representation of the best so far solution. See `current_internal` for a short explanation on the meaning of 'internal'
        """

    @property
    def current_best(self) -> RealSolution:
        """The current best-so-far solution."""

    @property
    def current_internal(self) -> RealSolution:
        """
        The internal representation of the last-evaluated solution. Note that, for a given problem instance, the input search point will be transformed with an automorphism and after evaluating the transformed point, the resulting objective value will also be transformed with another automorphism in the objective space. Such interal information are stored in this attribute.
        """

    @property
    def current(self) -> RealSolution:
        """The last-evaluated solution."""

    @property
    def y_unconstrained(self) -> float:
        """The current unconstrained value."""

    @property
    def y_unconstrained_best(self) -> float:
        """The current best unconstrained value."""

    @property
    def has_improved(self) -> bool:
        """Whether the last call to problem has caused global improvement."""

    def __repr__(self) -> str: ...

class IntegerState:
    def __init__(self, initial: IntegerSolution) -> None:
        """
        A state of a problem. Contains metrics such as the number of evaluations, and the current best value.

        Parameters
        ----------
        initial: Solution
            an object to initialize the state, which contains a placeholder for a solution to an optimization problem.
        """

    @property
    def evaluations(self) -> int:
        """The number of times the problem has been evaluated so far."""

    @property
    def optimum_found(self) -> bool:
        """
        Boolean value indicating whether final_target_found optimum of a given problem has been found.
        """

    @property
    def final_target_found(self) -> bool:
        """
        Boolean value indicating whether the final target of a given problem has been found.
        """

    @property
    def current_best_internal(self) -> IntegerSolution:
        """
        The internal representation of the best so far solution. See `current_internal` for a short explanation on the meaning of 'internal'
        """

    @property
    def current_best(self) -> IntegerSolution:
        """The current best-so-far solution."""

    @property
    def current_internal(self) -> IntegerSolution:
        """
        The internal representation of the last-evaluated solution. Note that, for a given problem instance, the input search point will be transformed with an automorphism and after evaluating the transformed point, the resulting objective value will also be transformed with another automorphism in the objective space. Such interal information are stored in this attribute.
        """

    @property
    def current(self) -> IntegerSolution:
        """The last-evaluated solution."""

    @property
    def y_unconstrained(self) -> float:
        """The current unconstrained value."""

    @property
    def y_unconstrained_best(self) -> float:
        """The current best unconstrained value."""

    @property
    def has_improved(self) -> bool:
        """Whether the last call to problem has caused global improvement."""

    def __repr__(self) -> str: ...

class MetaData:
    def __init__(self, problem_id: int, instance: int, name: str, n_variables: int, optimization_type: OptimizationType, final_target: float = 1e-08, fixed_dimension: bool = False, fixed_instance: bool = False) -> None:
        """
        Problem meta data. Contains static information about problems, such as their name and id.

        Parameters
        ----------
        problem_id: int
            The id of the problem
        instance: int
            The instance of the problem
        name: str
            The name of the problem
        n_variables: int
            The problem dimensionality
        optimization_type: OptimizationType
            Enum value, denoting if this is a maximization or minimization problem
        final_target: float
            The final target of the function
        fixed_dimension: bool
            Whether the problem has this dimension whatever dimension was requested
        fixed_instance: bool
            Whether the problem has this instance whatever instance was requested
        """

    @property
    def instance(self) -> int:
        """The instance number of the current problem"""

    @property
    def problem_id(self) -> int:
        """The id of the problem within its suite"""

    @property
    def name(self) -> str:
        """The name of the current problem"""

    @property
    def optimization_type(self) -> OptimizationType:
        """The type of problem (maximization or minimization)"""

    @property
    def n_variables(self) -> int:
        """The number of variables (dimension) of the current problem"""

    @property
    def final_target(self) -> float: ...

    @property
    def fixed_dimension(self) -> bool:
        """
        Whether the problem has one dimension, n_variables, whatever dimension was requested (the CEC2013 and graph problems)
        """

    @property
    def fixed_instance(self) -> bool:
        """
        Whether the problem has one instance, instance, whatever instance was requested (the CEC2013 and graph problems)
        """

    def __repr__(self) -> str: ...

    def __eq__(self, arg: MetaData, /) -> bool: ...

    __hash__: None = None

class LogInfo:
    def __init__(self, evaluations: int, raw_y: float, raw_y_best: float, transformed_y: float, transformed_y_best: float, y: float, y_best: float, x: Sequence[float], violations: Sequence[float], penalties: Sequence[float], optimum: RealSolution, has_improved: bool = False) -> None:
        """
        LogInfo struct. Gets passed to the call method of a logger when it is invoked.

        Parameters
        ----------
        evaluations: int
            The current number of evaluations
        y_best: float
            The raw current fitness value
        raw_y_best: float
            The raw current best fitness value
        transformed_y: float
            The transformed current fitness value
        transformed_y_best: float
            The transformed current best fitness value
        y: float
            The transformed current fitness value with constraints applied
        y_best: float
            The transformed current best fitness value with constraints applied
        x: np.npdarray | list
            The current solution passed to problem
        violation: np.npdarray | list
            The current solution constraint violations
        penalties: np.npdarray | list
            The current solution constraint penalties
        optimum: Solution
            The optimum solution for the problem
        has_improved: bool = True
            Whether the evaluation has caused an improvement in the objective value
        """

    @property
    def evaluations(self) -> int:
        """
        The number of function evaluations performed on the current problem so far
        """

    @property
    def raw_y(self) -> float: ...

    @property
    def raw_y_best(self) -> float: ...

    @property
    def transformed_y(self) -> float: ...

    @property
    def transformed_y_best(self) -> float: ...

    @property
    def y(self) -> float: ...

    @property
    def y_best(self) -> float: ...

    @property
    def x(self) -> list[float]: ...

    @property
    def violations(self) -> list[float]: ...

    @property
    def penalties(self) -> list[float]: ...

    @property
    def objective(self) -> RealSolution:
        """The best possible fitness value"""

    @property
    def has_improved(self) -> bool:
        """Whether the last call to problem has caused global improvement."""
