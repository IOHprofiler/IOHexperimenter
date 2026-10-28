"""Module that includes all IOHExperimenter problems"""

from collections.abc import Sequence
import enum
import os
from typing import Annotated, Final, overload

import numpy
from numpy.typing import NDArray

import ioh.iohcpp
import ioh.iohcpp.logger
from ioh.iohcpp.problem import sbox as sbox


class RealSingleObjective:
    def __init__(self, name: str, n_variables: int = 5, instance: int = 1, is_minimization: bool = True, bounds: ioh.iohcpp.RealBounds = ..., constraints: Sequence[ioh.iohcpp.AbstractRealConstraint] = [], optimum: ioh.iohcpp.RealSolution = ...) -> None:
        """Base class for RealSingleObjective problems"""

    def reset(self) -> None:
        """Reset all state variables of the problem."""

    def attach_logger(self, arg: ioh.iohcpp.logger.Logger, /) -> None:
        """
        Attach a logger to the problem to allow performance tracking.

        Parameters
        ----------
            logger: Logger
                A logger-object from the IOHexperimenter `logger` module.
        """

    def detach_logger(self) -> None:
        """Remove the specified logger from the problem."""

    @overload
    def __call__(self, arg: Annotated[NDArray[numpy.float64], dict(shape=(None,), order='C', device='cpu', writable=False)], /) -> float: ...

    @overload
    def __call__(self, arg: Sequence[float], /) -> float:
        """
        Evaluate the problem.

        Parameters
        ----------
            x: list
                the search point to evaluate. It must be a 1-dimensional array/list whose length matches search space's dimensionality
        Returns
        -------
        float
            The evaluated search point
        """

    @overload
    def __call__(self, arg: Annotated[NDArray[numpy.float64], dict(shape=(None, None), order='C', device='cpu', writable=False)], /) -> Annotated[NDArray[numpy.float64], dict(shape=(None,))]: ...

    @overload
    def __call__(self, arg: Sequence[Sequence[float]], /) -> Annotated[NDArray[numpy.float64], dict(shape=(None,))]:
        """
        Evaluate the problem at several search points.

        Parameters
        ----------
            x: list[list]
                the search points to evaluate, one per row. It must be a 2-dimensional array/list whose rows'
                length matches search space's dimensionality
        Returns
        -------
        numpy.ndarray
            The objective values of the search points: a new float64 array with one value per row
        """

    @overload
    @staticmethod
    def create(problem_name: str, instance_id: int, dimension: int) -> RealSingleObjective:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: a string indicating the problem name.
            instance_id: an integer identifier of the problem instance, which seeds the random generation
                of the tranformations in the search/objective spaces
            dimension: integer, representing the dimensionality of the search space
        """

    @overload
    @staticmethod
    def create(problem_id: int, instance_id: int, dimension: int) -> RealSingleObjective:
        """
        Create a problem instance

        Parameters
        ----------
            problem_id: the index of the problem to create.
            instance_id: an integer identifier of the problem instance, which seeds the random generation
                of the tranformations in the search/objective spaces
            dimension: integer, representing the dimensionality of the search space
        """

    problems: Final[dict[int, str]] = ...
    """All registered problems"""

    @property
    def log_info(self) -> ioh.iohcpp.LogInfo:
        """The data is that being sent to the logger."""

    @log_info.setter
    def log_info(self, arg: ioh.iohcpp.LogInfo, /) -> None: ...

    @property
    def state(self) -> ioh.iohcpp.RealState:
        """
        The current state of the optimization process containing, e.g., the current solution and the number of function evaluated consumed so far
        """

    @property
    def meta_data(self) -> ioh.iohcpp.MetaData:
        """
        The static meta-data of the problem containing, e.g., problem id, instance id, and problem's dimensionality
        """

    @property
    def optimum(self) -> ioh.iohcpp.RealSolution:
        """The optimum and its objective value for a problem instance"""

    @property
    def bounds(self) -> ioh.iohcpp.RealBounds:
        """The bounds of the problem."""

    @property
    def constraints(self) -> ioh.iohcpp.RealConstraintSet:
        """The constraints of the problem."""

    def add_constraint(self, arg: ioh.iohcpp.AbstractRealConstraint, /) -> None:
        """add a constraint"""

    def remove_constraint(self, arg: ioh.iohcpp.AbstractRealConstraint, /) -> None:
        """remove a constraint"""

    def enforce_bounds(self, weight: float = 1.0, how: ioh.iohcpp.ConstraintEnforcement = ioh.iohcpp.ConstraintEnforcement.SOFT, exponent: float = 1.0) -> None:
        """
        Enforced the bounds (box-constraints) as constraint
        Parameters
        ----------
            weight: The weight for computing the penalty (can be infinity to have strict box-constraints)
            how: The enforcement strategy, should be one of the 'ioh.ConstraintEnforcement' options
            exponent: The exponent for scaling the contraint
        """

    def set_id(self, new_problem_id: int) -> None:
        """update the problem id"""

    def set_instance(self, new_instance: int) -> None:
        """update the problem instance"""

    def set_name(self, new_name: str) -> None:
        """update the problem name"""

    def set_final_target(self, new_target: float) -> None:
        """update the final target"""

    def invert(self) -> None: ...

    def __repr__(self) -> str: ...

class IntegerSingleObjective:
    def __init__(self, name: str, n_variables: int = 5, instance: int = 1, is_minimization: bool = True, bounds: ioh.iohcpp.IntegerBounds = ..., constraints: Sequence[ioh.iohcpp.AbstractIntegerConstraint] = [], optimum: ioh.iohcpp.IntegerSolution = ...) -> None:
        """Base class for IntegerSingleObjective problems"""

    def reset(self) -> None:
        """Reset all state variables of the problem."""

    def attach_logger(self, arg: ioh.iohcpp.logger.Logger, /) -> None:
        """
        Attach a logger to the problem to allow performance tracking.

        Parameters
        ----------
            logger: Logger
                A logger-object from the IOHexperimenter `logger` module.
        """

    def detach_logger(self) -> None:
        """Remove the specified logger from the problem."""

    @overload
    def __call__(self, arg: Annotated[NDArray, dict(shape=(None,), order='C', device='cpu')], /) -> float: ...

    @overload
    def __call__(self, arg: Sequence[int], /) -> float:
        """
        Evaluate the problem.

        Parameters
        ----------
            x: list
                the search point to evaluate. It must be a 1-dimensional array/list whose length matches search space's dimensionality
        Returns
        -------
        float
            The evaluated search point
        """

    @overload
    def __call__(self, arg: Annotated[NDArray, dict(shape=(None, None), order='C', device='cpu')], /) -> Annotated[NDArray[numpy.float64], dict(shape=(None,))]: ...

    @overload
    def __call__(self, arg: Sequence[Sequence[int]], /) -> Annotated[NDArray[numpy.float64], dict(shape=(None,))]:
        """
        Evaluate the problem at several search points.

        Parameters
        ----------
            x: list[list]
                the search points to evaluate, one per row. It must be a 2-dimensional array/list whose rows'
                length matches search space's dimensionality
        Returns
        -------
        numpy.ndarray
            The objective values of the search points: a new float64 array with one value per row
        """

    @overload
    @staticmethod
    def create(problem_name: str, instance_id: int, dimension: int) -> IntegerSingleObjective:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: a string indicating the problem name.
            instance_id: an integer identifier of the problem instance, which seeds the random generation
                of the tranformations in the search/objective spaces
            dimension: integer, representing the dimensionality of the search space
        """

    @overload
    @staticmethod
    def create(problem_id: int, instance_id: int, dimension: int) -> IntegerSingleObjective:
        """
        Create a problem instance

        Parameters
        ----------
            problem_id: the index of the problem to create.
            instance_id: an integer identifier of the problem instance, which seeds the random generation
                of the tranformations in the search/objective spaces
            dimension: integer, representing the dimensionality of the search space
        """

    problems: Final[dict[int, str]] = ...
    """All registered problems"""

    @property
    def log_info(self) -> ioh.iohcpp.LogInfo:
        """The data is that being sent to the logger."""

    @log_info.setter
    def log_info(self, arg: ioh.iohcpp.LogInfo, /) -> None: ...

    @property
    def state(self) -> ioh.iohcpp.IntegerState:
        """
        The current state of the optimization process containing, e.g., the current solution and the number of function evaluated consumed so far
        """

    @property
    def meta_data(self) -> ioh.iohcpp.MetaData:
        """
        The static meta-data of the problem containing, e.g., problem id, instance id, and problem's dimensionality
        """

    @property
    def optimum(self) -> ioh.iohcpp.IntegerSolution:
        """The optimum and its objective value for a problem instance"""

    @property
    def bounds(self) -> ioh.iohcpp.IntegerBounds:
        """The bounds of the problem."""

    @property
    def constraints(self) -> ioh.iohcpp.IntegerConstraintSet:
        """The constraints of the problem."""

    def add_constraint(self, arg: ioh.iohcpp.AbstractIntegerConstraint, /) -> None:
        """add a constraint"""

    def remove_constraint(self, arg: ioh.iohcpp.AbstractIntegerConstraint, /) -> None:
        """remove a constraint"""

    def enforce_bounds(self, weight: float = 1.0, how: ioh.iohcpp.ConstraintEnforcement = ioh.iohcpp.ConstraintEnforcement.SOFT, exponent: float = 1.0) -> None:
        """
        Enforced the bounds (box-constraints) as constraint
        Parameters
        ----------
            weight: The weight for computing the penalty (can be infinity to have strict box-constraints)
            how: The enforcement strategy, should be one of the 'ioh.ConstraintEnforcement' options
            exponent: The exponent for scaling the contraint
        """

    def set_id(self, new_problem_id: int) -> None:
        """update the problem id"""

    def set_instance(self, new_instance: int) -> None:
        """update the problem instance"""

    def set_name(self, new_name: str) -> None:
        """update the problem name"""

    def set_final_target(self, new_target: float) -> None:
        """update the final target"""

    def invert(self) -> None: ...

    def __repr__(self) -> str: ...

class RealSingleObjectiveWrappedProblem(RealSingleObjective):
    pass

def wrap_real_problem(f: object, name: str, optimization_type: ioh.iohcpp.OptimizationType = ioh.iohcpp.OptimizationType.MIN, lb: float | None = None, ub: float | None = None, transform_variables: object | None = None, transform_objectives: object | None = None, calculate_objective: object | None = None, constraints: Sequence[ioh.iohcpp.AbstractRealConstraint] = []) -> None: ...

class IntegerSingleObjectiveWrappedProblem(IntegerSingleObjective):
    pass

def wrap_integer_problem(f: object, name: str, optimization_type: ioh.iohcpp.OptimizationType = ioh.iohcpp.OptimizationType.MIN, lb: float | None = None, ub: float | None = None, transform_variables: object | None = None, transform_objectives: object | None = None, calculate_objective: object | None = None, constraints: Sequence[ioh.iohcpp.AbstractIntegerConstraint] = []) -> None: ...

class BBOB(RealSingleObjective):
    """
    Black-Box Optimization Benchmarking (BBOB) problem set.

    Contains 24 noiselessreal-valued test functions supported on [-5, 5]^n, where n is the dimensionality.

    This problem was orginally proposed by Hansen et. al. in [FinckHRA10] and was implemented
    as the core component of the COmparing Continous Optimizer (COCO) platform [HansenARMTB20].

    We took the implementation of those 24 functions in
    https://github.com/numbbo/coco/tree/master/code-experiments/src (v2.2)
    and adopted those to our framework.

    We have acknowledged and specified in our license file
    https://github.com/IOHprofiler/IOHexperimenter/blob/master/LICENSE.md
    the usage and modification to the COCO/BBOB sources.

    Reference
    ---------
    [HansenARMTB20] Nikolaus Hansen, Anne Auger, Raymond Ros, Olaf Mersmann,
    Tea Tusar, and Dimo Brockhoff. "COCO: A platform for comparing continuous optimizers in
    a black-box setting." Optimization Methods and Software (2020): 1-31.

    [FinckHRA10] Steffen Finck, Nikolaus Hansen, Raymond Ros, and Anne Auger.
    "Real-parameter black-box optimization benchmarking 2009: Presentation of the noiseless functions."
    Technical Report 2009/20, Research Center PPE, 2009. Updated February, 2010.
    """

    @overload
    @staticmethod
    def create(problem_name: str, instance_id: int, dimension: int) -> BBOB:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: str
                a string indicating the problem name. 
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

    @overload
    @staticmethod
    def create(problem_id: int, instance_id: int, dimension: int) -> BBOB:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: int
                a string indicating the problem name. 
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

class Sphere(BBOB):
    """Sphere function"""

    def __init__(self, instance: int, n_variables: int) -> None: ...

class Ellipsoid(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Rastrigin(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class BuecheRastrigin(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class LinearSlope(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class AttractiveSector(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class StepEllipsoid(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Rosenbrock(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class RosenbrockRotated(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class EllipsoidRotated(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Discus(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class BentCigar(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class SharpRidge(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class DifferentPowers(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class RastriginRotated(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Weierstrass(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Schaffers10(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Schaffers1000(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class GriewankRosenbrock(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Schwefel(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Gallagher101(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Gallagher21(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Katsuura(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class LunacekBiRastrigin(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class SBOX(RealSingleObjective):
    """
    Black-Box Optimization Benchmarking (BBOB) problem set.

    Contains 24 noiselessreal-valued test functions supported on [-5, 5]^n, where n is the dimensionality.

    This problem was orginally proposed by Hansen et. al. in [FinckHRA10] and was implemented
    as the core component of the COmparing Continous Optimizer (COCO) platform [HansenARMTB20].

    We took the implementation of those 24 functions in
    https://github.com/numbbo/coco/tree/master/code-experiments/src (v2.2)
    and adopted those to our framework.

    We have acknowledged and specified in our license file
    https://github.com/IOHprofiler/IOHexperimenter/blob/master/LICENSE.md
    the usage and modification to the COCO/BBOB sources.

    Reference
    ---------
    [HansenARMTB20] Nikolaus Hansen, Anne Auger, Raymond Ros, Olaf Mersmann,
    Tea Tusar, and Dimo Brockhoff. "COCO: A platform for comparing continuous optimizers in
    a black-box setting." Optimization Methods and Software (2020): 1-31.

    [FinckHRA10] Steffen Finck, Nikolaus Hansen, Raymond Ros, and Anne Auger.
    "Real-parameter black-box optimization benchmarking 2009: Presentation of the noiseless functions."
    Technical Report 2009/20, Research Center PPE, 2009. Updated February, 2010.
    """

    @overload
    @staticmethod
    def create(problem_name: str, instance_id: int, dimension: int) -> SBOX:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: str
                a string indicating the problem name. 
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

    @overload
    @staticmethod
    def create(problem_id: int, instance_id: int, dimension: int) -> SBOX:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: int
                a string indicating the problem name. 
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

class PBO(IntegerSingleObjective):
    """
    Pseudo-Boolean Optimization (PBO) problem set.

    Contains 25 test functions taking their domain on {0, 1}^n, where n is the length of bitstrings.

    In PBO, we cover some theory-motivated function, e.g., OneMax and LeadingOnes
    as well as others with more practical relevance, e.g., the NK Landscape [DoerrYHWSB20].
    We also utilized the so-called W-model for generating/enriching the problem set [WeiseW18].

    Reference
    ---------
    [DoerrYHWSB20] Carola Doerr, Furong Ye, Naama Horesh, Hao Wang, Ofer M. Shir, and Thomas Bäck.
    "Benchmarking discrete optimization heuristics with IOHprofiler." Applied Soft Computing 88 (2020): 106027.

    [WeiseW18] Thomas Weise and Zijun Wu. "Difficult features of combinatorial optimization problems and
    the tunable w-model benchmark problem for simulating them." In Proceedings of the Genetic
    and Evolutionary Computation Conference Companion, pp. 1769-1776. 2018.
    """

    @overload
    @staticmethod
    def create(problem_name: str, instance_id: int, dimension: int) -> PBO:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: str
                a string indicating the problem name. 
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

    @overload
    @staticmethod
    def create(problem_id: int, instance_id: int, dimension: int) -> PBO:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: int
                a string indicating the problem name. 
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

class OneMax(PBO):
    """
    OneMax:
    {0,1}^n → [0..n], x ↦ ∑_{i=1}^n x_i.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnes(PBO):
    """
    LeadingOnes:
    {0,1}^n → [0..n], x ↦ max{i∈[0..n] ∣ ∀j≤i: x_j=1}
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class Linear(PBO):
    """
    A Linear Function with Harmonic Weights:
    {0,1}^n → ℝ, x ↦ ∑_i i * x_i
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMaxDummy1(PBO):
    """
    A variant of OneMax applying the Dummy transformation of W-model. m = 0.5n.
    Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMaxDummy2(PBO):
    """
    A variant of OneMax applying the Dummy transformation of W-model. m = 0.9n.
    Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMaxNeutrality(PBO):
    """
    A variant of OneMax applying the Neutrality transformation of W-model. \\mu = 3.
    Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMaxEpistasis(PBO):
    """
    A variant of OneMax applying the Epistasis transformation of W-model. \\nu = 4.
    Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMaxRuggedness1(PBO):
    """
    A variant of OneMax applying the first Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMaxRuggedness2(PBO):
    """
    A variant of OneMax applying the second Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMaxRuggedness3(PBO):
    """
    A variant of OneMax applying the third Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnesDummy1(PBO):
    """
    A variant of LeadingOnes applying the Dummy transformation of W-model. m = 0.5n.
    Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnesDummy2(PBO):
    """
    A variant of LeadingOnes applying the Dummy transformation of W-model. m = 0.9n.
    Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnesNeutrality(PBO):
    """
    A variant of LeadingOnes applying the Neutrality transformation of W-model. \\mu = 3.
    Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnesEpistasis(PBO):
    """
    A variant of LeadingOnes applying the Epistasis transformation of W-model. \\nu = 4.
    Details can be found in https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnesRuggedness1(PBO):
    """
    A variant of LeadingOnes applying the first Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnesRuggedness2(PBO):
    """
    A variant of LeadingOnes applying the second Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnesRuggedness3(PBO):
    """
    A variant of LeadingOnes applying the third Ruggnedness transformation in https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class LABS(PBO):
    """
    Low Autocorrelation Binary Sequences (LABS):
    x ↦ n^2 / 2∑_{k=1}^{n-1}(∑_{i=1}^{n−k}s_is_{i+k})^2, where s_i = 2x_i − 1
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class IsingRing(PBO):
    """
    The Ising Spin Glass model arose in solid-state physics and statistical mechanics, aiming to 
    describe simple interactions within many-particle systems. A compact form used here is 
    x ↦ ∑_{{u,v} \\in E}[x_ux_v - (1-x_u) (1-x_v)]. E is defined over a one-dimensional lattice.
    Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class IsingTorus(PBO):
    """
    The Ising Spin Glass model arose in solid-state physics and statistical mechanics, aiming to 
    describe simple interactions within many-particle systems. A compact form used here is 
    x ↦ ∑_{{u,v} \\in E}[x_ux_v - (1-x_u) (1-x_v)]. E is defined over a two-dimensional lattice.
    Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class IsingTriangular(PBO):
    """
    The Ising Spin Glass model arose in solid-state physics and statistical mechanics, aiming to 
    describe simple interactions within many-particle systems. A compact form used here is 
    x ↦ ∑_{{u,v} \\in E}[x_ux_v - (1-x_u) (1-x_v)]. E is defined over a three-dimensional lattice.
    Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class MIS(PBO):
    """
    The maximum independent vertex set (MIVS) formulated as 
    x ↦ ∑_i x_i - n ∑_{i,j} x_i x_j e_{i,j}, where e_{i,j} = 1 if {i,j} \\in E, otherwise e_{i,j} = 0.
    Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class NQueens(PBO):
    """
    The N-queens problem (NQP) is defined as the task to place N queens on an N*N chessboard in such a 
    way that they cannot attack each other.   
    Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class ConcatenatedTrap(PBO):
    """
    Concatenated Trap (CT) is defined by partitioning a bit-string into segments of length k and 
    concatenating m = n/k trap functions that takes each segment as input. The trap function is 
    defined as follows: f_trap(u) = 1 if the k number u of ones satisfies u = k and 
    f_trap(u) = (k−1−u)/k otherwise. We use k = 5 in our experiments.
    Details can be found in  https://doi.org/10.1016/j.asoc.2019.106027.
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class CEC2013(RealSingleObjective):
    """Functions from the CEC2013 conference."""

    @overload
    @staticmethod
    def create(problem_name: str, instance_id: int, dimension: int) -> CEC2013:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: str
                a string indicating the problem name.
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

    @overload
    @staticmethod
    def create(problem_id: int, instance_id: int, dimension: int) -> CEC2013:
        """
        Create a problem instance

        Parameters
        ----------
            problem_id: int
                a number indicating the problem numeric identifier.
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

    @property
    def n_optima(self) -> int: ...

    @property
    def optima(self) -> list[ioh.iohcpp.RealSolution]:
        """
        The optima of the problem, as a list of new RealSolution objects (copies)
        """

    @property
    def single_global_optimum(self) -> bool: ...

    @property
    def sphere_limit(self) -> float: ...

    @property
    def rho(self) -> float: ...

    def set_optimum(self, i: int, single_global: bool = True) -> None: ...

class EqualMaxima(CEC2013):
    pass

class FivePeaks(CEC2013):
    pass

class ModifiedRastrigin(CEC2013):
    pass

class Shubert(CEC2013):
    pass

class SixHumpCamelback(CEC2013):
    pass

class UnevenEqualMaxima(CEC2013):
    pass

class Vincent(CEC2013):
    pass

class CEC2013CompositionFunction(CEC2013):
    pass

class CEC2022(RealSingleObjective):
    """Functions from the CEC2022 2022 conference."""

    @overload
    @staticmethod
    def create(problem_name: str, instance_id: int, dimension: int) -> CEC2022:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: str
                a string indicating the problem name.
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

    @overload
    @staticmethod
    def create(problem_id: int, instance_id: int, dimension: int) -> CEC2022:
        """
        Create a problem instance

        Parameters
        ----------
            problem_id: int
                a number indicating the problem numeric identifier.
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

class CEC2022Zakharov(CEC2022):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class CEC2022Rosenbrock(CEC2022):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class CEC2022SchafferF7(CEC2022):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class CEC2022Rastrigin(CEC2022):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class CEC2022Levy(CEC2022):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class CEC2022HybridFunction1(CEC2022):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class CEC2022HybridFunction2(CEC2022):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class CEC2022HybridFunction3(CEC2022):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class CEC2022CompositionFunction1(CEC2022):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class CEC2022CompositionFunction2(CEC2022):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class CEC2022CompositionFunction3(CEC2022):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class CEC2022CompositionFunction4(CEC2022):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class DynamicBinVal(IntegerSingleObjective):
    @overload
    @staticmethod
    def create(problem_name: str, instance_id: int, dimension: int) -> DynamicBinVal:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: str
                a string indicating the problem name.
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

    @overload
    @staticmethod
    def create(problem_id: int, instance_id: int, dimension: int) -> DynamicBinVal:
        """
        Create a problem instance

        Parameters
        ----------
            problem_id: int
                a number indicating the problem numeric identifier.
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

    def step(self) -> int:
        """
        Step the dynamic binary value problem forward by one timestep, and permute the weights randomly.

        Returns
        -------
        int
            The current timestep after the step.
        """

    @property
    def time_step(self) -> int: ...

    @property
    def weights(self) -> Annotated[NDArray[numpy.float64], dict(shape=(None,))]:
        """The current weights, as a new numpy array (float64)"""

class DynamicBinValUniform(DynamicBinVal):
    """
    Dynamic BinVal. Details: https://link.springer.com/article/10.1007/s42979-022-01203-z
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class DynamicBinValPowersOfTwo(DynamicBinVal):
    """
    Dynamic BinVal. Details: https://link.springer.com/article/10.1007/s42979-022-01203-z
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

class DynamicBinValPareto(DynamicBinVal):
    """
    Dynamic BinVal. Details: https://link.springer.com/article/10.1007/s42979-022-01203-z
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

    @property
    def pareto_shape(self) -> float: ...

class DynamicBinValRanking(DynamicBinVal):
    """
    Dynamic BinVal. Details: https://link.springer.com/article/10.1007/s42979-022-01203-z
    """

    def __init__(self, instance: int, n_variables: int) -> None: ...

    @overload
    def rank(self, arg: Annotated[NDArray, dict(shape=(None, None), order='C', device='cpu')], /) -> Annotated[NDArray[numpy.int32], dict(shape=(None, None))]: ...

    @overload
    def rank(self, arg: Sequence[Sequence[int]], /) -> Annotated[NDArray[numpy.int32], dict(shape=(None, None))]:
        """
        Sort bitstrings in lexicographical order, by the comparison ordering. Each bitstring is evaluated once.

        Parameters
        ----------
        bitstrings : list
            The bitstrings to sort, one per row (a list of lists or a two-dimensional array).

        Returns
        -------
        numpy.ndarray
            The bitstrings in sorted order, one per row: a new two-dimensional int32 array (the argument is not
            changed).
        """

    @overload
    def rank_indices(self, arg: Annotated[NDArray, dict(shape=(None, None), order='C', device='cpu')], /) -> Annotated[NDArray[numpy.int32], dict(shape=(None,))]: ...

    @overload
    def rank_indices(self, arg: Sequence[Sequence[int]], /) -> Annotated[NDArray[numpy.int32], dict(shape=(None,))]:
        """
        The order of bitstrings sorted in lexicographical order, by the comparison ordering. Each bitstring is
        evaluated once.

        Parameters
        ----------
        bitstrings : list
            The bitstrings to sort, one per row (a list of lists or a two-dimensional array).

        Returns
        -------
        numpy.ndarray
            The indices of the bitstrings (rows) in sorted order: a new int32 array.
        """

    def get_comparison_ordering(self) -> Annotated[NDArray[numpy.int32], dict(shape=(None,))]:
        """
        Get the current comparison ordering vector.

        Returns
        -------
        numpy.ndarray
            The current state of the comparison ordering vector: a new int32 array (step() changes the
            ordering, not this array).
        """

class AbstractWModel(IntegerSingleObjective):
    """
    An abstract W-model class. Please apply the WModelOneMax and WModelLeadingOnes classes.

    W-model problems applies four basic transformations: reduction of dummy variables,
    neutrality, epistasis, and fitness perturbation on benchmark problems. Each transformation
    is configured by the corresponding argument dummy_select_rate, epistasis_block_size,
    neutrality_mu, and ruggedness_gamma.

    The details of the transformations can be found in
    https://www.sciencedirect.com/science/article/pii/S1568494619308099

    We have acknowledged Thomas Weise, proposing the original W-model
    https://dl.acm.org/doi/abs/10.1145/3205651.3208240 and having discussions supporting this work.

    Reference
    ---------
    [WeiseW18] Thomas Weise and Zijun Wu. "Difficult features of combinatorial optimization problems
    and the tunable w-model benchmark problem for simulating them." Proc. of the Genetic and 
    Evolutionary Computation Conference Companion, 1769-1776, 2018

    [DoerrYHWSB20] Carola Doerr, Furong Ye, Naama Horesh, Hao Wang, Ofer M. Shir, Thomas Bäck.
    "Benchmarking discrete optimization heuristics with IOHprofiler."
    Applied Soft Computing, 88: 106027, 2020
    """

    def __init__(self, problem_id: int, instance: int, n_variables: int, name: str, dummy_select_rate: float = 0.0, epistasis_block_size: int = 0, neutrality_mu: int = 0, ruggedness_gamma: int = 0) -> None: ...

    @overload
    def wmodel_evaluate(self, arg: Annotated[NDArray, dict(shape=(None,), order='C', device='cpu')], /) -> int: ...

    @overload
    def wmodel_evaluate(self, arg: Sequence[int], /) -> int: ...

class WModelLeadingOnes(AbstractWModel):
    """
    A W-model problem built on LeadingOnes.

    This W-model problem applies four basic transformations: reduction of dummy variables,
    neutrality, epistasis, and fitness perturbation on the LeadingOnes problem. Each transformation
    is configured by the corresponding argument dummy_select_rate, epistasis_block_size,
    neutrality_mu, and ruggedness_gamma.

    The definitions of LeadingOnes and the transformations can be found in
    https://www.sciencedirect.com/science/article/pii/S1568494619308099

    We have acknowledged Thomas Weise, proposing the original W-model
    https://dl.acm.org/doi/abs/10.1145/3205651.3208240 and having discussions supporting this work.

    Reference
    ---------
    [WeiseW18] Thomas Weise and Zijun Wu. "Difficult features of combinatorial optimization problems
    and the tunable w-model benchmark problem for simulating them." Proc. of the Genetic and 
    Evolutionary Computation Conference Companion, 1769-1776, 2018

    [DoerrYHWSB20] Carola Doerr, Furong Ye, Naama Horesh, Hao Wang, Ofer M. Shir, Thomas Bäck.
    "Benchmarking discrete optimization heuristics with IOHprofiler."
    Applied Soft Computing, 88: 106027, 2020
    """

    def __init__(self, instance: int, n_variables: int, dummy_select_rate: float = 0.0, epistasis_block_size: int = 0, neutrality_mu: int = 0, ruggedness_gamma: int = 0) -> None: ...

class WModelOneMax(AbstractWModel):
    """
    A W-model problem built on OneMax.

    This W-model problem applies four basic transformations: reduction of dummy variables,
    neutrality, epistasis, and fitness perturbation on the OneMax problem. Each transformation
    is configured by the corresponding argument dummy_select_rate, epistasis_block_size,
    neutrality_mu, and ruggedness_gamma.

    The definitions of OneMax and the transformations can be found in
    https://www.sciencedirect.com/science/article/pii/S1568494619308099

    We have acknowledged Thomas Weise, proposing the original W-model
    https://dl.acm.org/doi/abs/10.1145/3205651.3208240 and having discussions supporting this work.

    Reference
    ---------
    [WeiseW18] Thomas Weise and Zijun Wu. "Difficult features of combinatorial optimization problems
    and the tunable w-model benchmark problem for simulating them." Proc. of the Genetic and 
    Evolutionary Computation Conference Companion, 1769-1776, 2018

    [DoerrYHWSB20] Carola Doerr, Furong Ye, Naama Horesh, Hao Wang, Ofer M. Shir, Thomas Bäck.
    "Benchmarking discrete optimization heuristics with IOHprofiler."
    Applied Soft Computing, 88: 106027, 2020
    """

    def __init__(self, instance: int, n_variables: int, dummy_select_rate: float = 0.0, epistasis_block_size: int = 0, neutrality_mu: int = 0, ruggedness_gamma: int = 0) -> None: ...

class GraphConstraint(ioh.iohcpp.AbstractIntegerConstraint):
    def __repr__(self) -> str: ...

class PWTConstraint(ioh.iohcpp.AbstractIntegerConstraint):
    def __repr__(self) -> str: ...

class GraphMeta:
    """Graph metadata"""

    @property
    def edge_file(self) -> str:
        """File with edge data"""

    @property
    def edge_weights(self) -> str:
        """File with edge weights"""

    @property
    def vertex_weights(self) -> str:
        """File with vertex weights"""

    @property
    def constraint_weights(self) -> str:
        """File with constraint weights"""

    @property
    def constraint_variances(self) -> str:
        """File with constraint variances"""

    @property
    def is_edge(self) -> bool:
        """Edge type flag"""

    @property
    def digraph(self) -> bool:
        """Is digraph flag"""

    @property
    def constraint_limit(self) -> float:
        """Maximum value of the constraint"""

    @property
    def n_vertices(self) -> int:
        """Number of vertices"""

class Graph:
    """Graph data structure"""

    def __repr__(self) -> str: ...

    def dimension(self) -> int:
        """Get the dimension of the graph"""

    @property
    def meta(self) -> GraphMeta:
        """Graph metadata"""

    @property
    def loaded(self) -> bool:
        """Whether the graph is loaded in memory"""

    @property
    def adjacency_list(self) -> list[list[tuple[int, float]]]:
        """Adjacency list representation of the graph"""

    @property
    def edges(self) -> list[tuple[int, int, float]]:
        """Edge list representation of the graph"""

    @property
    def constraint_weights(self) -> Annotated[NDArray[numpy.float64], dict(shape=(None,), writable=False)]:
        """Constraint weights (a read-only numpy array)"""

    @property
    def constraint_variances(self) -> Annotated[NDArray[numpy.float64], dict(shape=(None,), writable=False)]:
        """Constraint variances (a read-only numpy array)"""

    @property
    def edge_weights(self) -> Annotated[NDArray[numpy.float64], dict(shape=(None,), writable=False)]:
        """Edge weights (a read-only numpy array)"""

    @property
    def vertex_weights(self) -> Annotated[NDArray[numpy.float64], dict(shape=(None,), writable=False)]:
        """Vertex weights (a read-only numpy array)"""

class GraphProblem(IntegerSingleObjective):
    """
    Graph-based problems (submodular problems)

    These submodular problems are all based on an underlying graph structure. 

    Reference
    ---------
    [Neumann23] Neumann, Frank, Aneta Neumann, Chao Qian, Viet Anh Do, Jacob de Nobel, Diederick Vermetten, 
    Saba Sadeghi Ahouei, Furong Ye, Hao Wang, and Thomas Bäck. 
    "Benchmarking Algorithms for Submodular Optimization Problems Using IOHProfiler." 
    arXiv preprint arXiv:2302.01464 (2023).
    """

    @overload
    @staticmethod
    def create(problem_name: str, instance_id: int, dimension: int) -> GraphProblem:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: str
                a string indicating the problem name. 
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

    @overload
    @staticmethod
    def create(problem_id: int, instance_id: int, dimension: int) -> GraphProblem:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: int
                a string indicating the problem name. 
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

    @property
    def graph(self) -> Graph:
        """The underlying graph structure"""

class MaxCut(GraphProblem):
    """
    Max-Cut problems

    The maximum cut problem is a classical NP-hard problem and can be defined as follows. Given an undirected
    weighted graph G = (V, E, w) with weights w: E → R≥0 on the edges, the goal is to select a set V1 ⊆ V such that
    the sum of the weight of edges between V1 and V2 = V \\ V1 is maximal.

    For a given search point x ∈ {0, 1}^n where n = |V|, we have V1(x) = {vi | xi = 1} and V2(x) = {vi | xi = 0}. 
    Let C(x) = {e ∈ E | e ∩ V1(x) 6= ∅ ∧ e ∩ V2(x) 6= ∅} be the cut of a given search point x. The goal is to maximize
    f'(x) = Sum_{e∈C(x)} w(e).

    Reference
    ---------
    [Neumann23] Neumann, Frank, Aneta Neumann, Chao Qian, Viet Anh Do, Jacob de Nobel, Diederick Vermetten, 
    Saba Sadeghi Ahouei, Furong Ye, Hao Wang, and Thomas Bäck. 
    "Benchmarking Algorithms for Submodular Optimization Problems Using IOHProfiler." 
    arXiv preprint arXiv:2302.01464 (2023).
    """

    @staticmethod
    def load_instances(path: str | os.PathLike | None = None) -> dict[int, str]:
        """
        Adds the graphs of a definitions file after the existing graph problems, in GraphProblem and
        IntegerSingleObjective, and returns their {id: name}. Their ids continue after the highest graph problem id, and
        their names are the class name and the id. Loading the same file again returns the same ids. Calling it without
        a path is deprecated and does nothing.
        """

class MaxCoverage(GraphProblem):
    """
    Max-Coverage problems

    The maximum coverage problem is a classical optimization problem on graphs. Given an undirected weighted
    graph G = (V, E, c) with costs c : V → R≥0 on the vertices. We denote by N(V') = 
    {vi | ∃e ∈ E : e ∩ V' != ∧ e ∩ vi != ∅} the set of all nodes of V' and their neighbors in G.
    For a given search point x ∈ {0, 1}^n where n = |V |, we have V'(x) = {vi | xi = 1} and 
    c(x) = Sum_{v∈V 0(x)} c(v).

    In the deterministic setting, the goal is to maximize f(x) = |N(V'(x))|
    under the constraint that c(x) ≤ B holds.

    Reference
    ---------
    [Neumann23] Neumann, Frank, Aneta Neumann, Chao Qian, Viet Anh Do, Jacob de Nobel, Diederick Vermetten, 
    Saba Sadeghi Ahouei, Furong Ye, Hao Wang, and Thomas Bäck. 
    "Benchmarking Algorithms for Submodular Optimization Problems Using IOHProfiler." 
    arXiv preprint arXiv:2302.01464 (2023).
    """

    @staticmethod
    def load_instances(path: str | os.PathLike | None = None) -> dict[int, str]:
        """
        Adds the graphs of a definitions file after the existing graph problems, in GraphProblem and
        IntegerSingleObjective, and returns their {id: name}. Their ids continue after the highest graph problem id, and
        their names are the class name and the id. Loading the same file again returns the same ids. Calling it without
        a path is deprecated and does nothing.
        """

class MaxInfluence(GraphProblem):
    """
    Max-Influence problems

    The maximum influence problem in social networks is an important submodular optimization problem that has been
    widely studied in the literature from various perspectives. Let a directed graph G(V, E) represent a social network, 
    where each node is a user and each edge (u, v) ∈ E has a probability pu,v representing the strength of influence from user u to v.

    A fundamental propagation model is independence cascade. Starting from a seed set X, it uses a set At to record the
    nodes activated at time t, and at time t + 1, each inactive neighbor v of u ∈ At becomes active with probability pu,v.
    This process is repeated until no nodes get activated at some time. The set of nodes activated by propagating from X
    is denoted as IC(X), which is a random variable.

    The goal is to maximize the expected value of IC(X). Note that the computation of the expected value is done by
    running a simulation of the influence process several times and averaging its results. In this sense, the computation of
    the objective function value is stochastic.

    For a given search point x ∈ {0, 1}^n where n = |V|, we have V'(x) = {vi | xi = 1} and c(x) = SUM_{v∈V 0(x)} c(v).


    Reference
    ---------
    [Neumann23] Neumann, Frank, Aneta Neumann, Chao Qian, Viet Anh Do, Jacob de Nobel, Diederick Vermetten, 
    Saba Sadeghi Ahouei, Furong Ye, Hao Wang, and Thomas Bäck. 
    "Benchmarking Algorithms for Submodular Optimization Problems Using IOHProfiler." 
    arXiv preprint arXiv:2302.01464 (2023).
    """

    @staticmethod
    def load_instances(path: str | os.PathLike | None = None) -> dict[int, str]:
        """
        Adds the graphs of a definitions file after the existing graph problems, in GraphProblem and
        IntegerSingleObjective, and returns their {id: name}. Their ids continue after the highest graph problem id, and
        their names are the class name and the id. Loading the same file again returns the same ids. Calling it without
        a path is deprecated and does nothing.
        """

class PackWhileTravel(GraphProblem):
    """
    Packing while traveling problems

    The packing while traveling (PWT) problem is a non-monotone submodular optimization problem which is
    obtained from the traveling thief problem (TTP) when the route is fixed.

    The input is given as n + 1 cities with distances di, 1 ≤ i ≤ n, from city i to city i + 1. Each city i, 1 ≤ i ≤ n,
    contains a set of items Mi ⊆ M, |Mi| = mi. Each item eij ∈ Mi, 1 ≤ j ≤ mi, has a positive integer profit pij and
    weight wij . A fixed route N = (1, 2, ..., n + 1) is traveled by a vehicle with velocity v ∈ [vmin, vmax]. We denote by
    xij ∈ {0, 1} the variable indicating whether or not item eij is chosen in a solution 
    x = (x11, x12, ..., x1m1, x21, ..., xnmn) ∈ {0, 1}^m, where m = SUM_{n}^{i=1} mi. 
    The total benefit of selecting a subset of items selected by x is given as PWT(x) = P(x) − R · T(x),
    where P(x) is the total profit of the selected items and T(x) is the total travel time for the vehicle carrying the selected
    items. Formally, we have P(x) = Sum_{i=1}^n Sum_{j=1}^{mi} pij xij and
    T(x) = Sum_{i=1}^n di / (vmax − ν Sum_{k=1}^i Sum_{j=1}^{mk} wkj xkj
    Here, ν = (vmax−vmin) / B is a constant defined by the input parameters, where B is the capacity of the vehicle. The
    problem is already NP-hard without any additional constraints, but often considered with a typical knapsack
    constraint given as c(x) = Sum_{i=1}^n Sum_{j=1}^{mi} wij xij ≤ B.
    As fitness functions, we use g(x) = (f'(x), c(x)) with
    f'(x) = PWT(x) if c(x) ≤ B
    and f'(x) = B − c(x) − R · T(vmin) if c(x) > B
    where T(vmin) = (1 / vmin) Sum_i=1}^n di is the travel time at speed vmin.


    Reference
    ---------
    [Neumann23] Neumann, Frank, Aneta Neumann, Chao Qian, Viet Anh Do, Jacob de Nobel, Diederick Vermetten, 
    Saba Sadeghi Ahouei, Furong Ye, Hao Wang, and Thomas Bäck. 
    "Benchmarking Algorithms for Submodular Optimization Problems Using IOHProfiler." 
    arXiv preprint arXiv:2302.01464 (2023).
    """

    @staticmethod
    def load_instances(path: str | os.PathLike | None = None) -> dict[int, str]:
        """
        Adds the graphs of a definitions file after the existing graph problems, in GraphProblem and
        IntegerSingleObjective, and returns their {id: name}. Their ids continue after the highest graph problem id, and
        their names are the class name and the id. Loading the same file again returns the same ids. Calling it without
        a path is deprecated and does nothing.
        """

class StarDiscrepancySampler(enum.Enum):
    """
    Methods which can be used to sample the initial grids for the star discrepancy problems
    """

    UNIFORM = 0

    HALTON = 2

    SOBOL = 1

    def __int__(self) -> int: ...

    def __index__(self) -> int: ...

UNIFORM: StarDiscrepancySampler = StarDiscrepancySampler.UNIFORM

HALTON: StarDiscrepancySampler = StarDiscrepancySampler.HALTON

SOBOL: StarDiscrepancySampler = StarDiscrepancySampler.SOBOL

class RealStarDiscrepancy(RealSingleObjective):
    """
    Star-discrepancy problems

    The 𝐿_infinity star discrepancy is a measure for the regularity of a finite set
    of points taken from [0, 1)^d. Calculating this measure is challenging, and exact algorithm
    fall short even for relatively small grids. This suite provides access to a set of these problems
    to tackle the problem from an optimization perspective.

    There are two equivalent representations for these problems: Integer-based and Real-valued. 

    For the Real-valued, the goal is to find the a point in [0,1)^d which minimizes the resulting discepancy directly.

    The Integer-valued version takes advantage of the fact that for each dimension, the optimium 
    has a value matching an existing point int the grid. As such, the search for a point in the grid can 
    be reduced to finding in each dimension the index of the coordinate, resulting in a search domain 
    [0,n]^d, where n is the number of gridpoints. 


    Reference
    ---------
    [Clement23] Clement, Francois, Diederick Vermetten, Jacob de Nobel, Alexandre Jesus, 
    Luís Paquete, and Carola Doerr. "Computing Star Discrepancies with 
    Numerical Black-Box Optimization Algorithms.\"
    """

    def __init__(self, instance: int = 1, n_variables: int = 5, n_samples: int = 5, sampler_type: StarDiscrepancySampler = StarDiscrepancySampler.UNIFORM) -> None:
        """
        Star Discrepancy Problems

        Parameters
        ----------
        instance: int = 1
            The instance of the problem, will be used as seed for sampler
        n_variables: int = 5
            The dimension of the problem
        n_samples: int = 5
            The number of sampled points
        sampler_type: StarDiscrepancySampler
            The type of sampler to use when sampling the points
        """

    @property
    def grid(self) -> Annotated[NDArray[numpy.float64], dict(shape=(None, None))]:
        """
        The grid of the problem, as a new (n_samples, n_variables) numpy array (float64)
        """

    @overload
    @staticmethod
    def create(problem_name: str, instance_id: int, dimension: int) -> RealStarDiscrepancy:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: str
                a string indicating the problem name. 
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

    @overload
    @staticmethod
    def create(problem_id: int, instance_id: int, dimension: int) -> RealStarDiscrepancy:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: int
                a string indicating the problem name. 
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

class IntegerStarDiscrepancy(IntegerSingleObjective):
    """
    Star-discrepancy problems

    The 𝐿_infinity star discrepancy is a measure for the regularity of a finite set
    of points taken from [0, 1)^d. Calculating this measure is challenging, and exact algorithm
    fall short even for relatively small grids. This suite provides access to a set of these problems
    to tackle the problem from an optimization perspective.

    There are two equivalent representations for these problems: Integer-based and Real-valued. 

    For the Real-valued, the goal is to find the a point in [0,1)^d which minimizes the resulting discepancy directly.

    The Integer-valued version takes advantage of the fact that for each dimension, the optimium 
    has a value matching an existing point int the grid. As such, the search for a point in the grid can 
    be reduced to finding in each dimension the index of the coordinate, resulting in a search domain 
    [0,n]^d, where n is the number of gridpoints. 


    Reference
    ---------
    [Clement23] Clement, Francois, Diederick Vermetten, Jacob de Nobel, Alexandre Jesus, 
    Luís Paquete, and Carola Doerr. "Computing Star Discrepancies with 
    Numerical Black-Box Optimization Algorithms.\"
    """

    def __init__(self, instance: int = 1, n_variables: int = 5, n_samples: int = 5, sampler_type: StarDiscrepancySampler = StarDiscrepancySampler.UNIFORM) -> None:
        """
        Star Discrepancy Problems

        Parameters
        ----------
        instance: int = 1
            The instance of the problem, will be used as seed for sampler
        n_variables: int = 5
            The dimension of the problem
        n_samples: int = 5
            The number of sampled points
        sampler_type: StarDiscrepancySampler
            The type of sampler to use when sampling the points
        """

    @property
    def grid(self) -> Annotated[NDArray[numpy.float64], dict(shape=(None, None))]:
        """
        The grid of the problem, as a new (n_samples, n_variables) numpy array (float64)
        """

    @overload
    @staticmethod
    def create(problem_name: str, instance_id: int, dimension: int) -> IntegerStarDiscrepancy:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: str
                a string indicating the problem name. 
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

    @overload
    @staticmethod
    def create(problem_id: int, instance_id: int, dimension: int) -> IntegerStarDiscrepancy:
        """
        Create a problem instance

        Parameters
        ----------
            problem_name: int
                a string indicating the problem name. 
            instance_id: int
                an integer identifier of the problem instance
            dimension: int
                the dimensionality of the search space
        """

class ManyAffine(RealSingleObjective):
    @overload
    def __init__(self, instance: int, n_variables: int) -> None: ...

    @overload
    def __init__(self, xopt: Annotated[NDArray[numpy.float64], dict(shape=(None,), order='C', device='cpu', writable=False)], weights: Sequence[float], instances: Sequence[int], n_variables: int, scale_factors: Sequence[float] = ...) -> None: ...

    @overload
    def __init__(self, xopt: Sequence[float], weights: Sequence[float], instances: Sequence[int], n_variables: int, scale_factors: Sequence[float] = ...) -> None: ...

    @property
    def weights(self) -> Annotated[NDArray[numpy.float64], dict(shape=(None,))]:
        """The weights of the 24 BBOB functions, as a new numpy array (float64)"""

    @property
    def instances(self) -> Annotated[NDArray[numpy.int32], dict(shape=(None,))]:
        """The instances of the 24 BBOB functions, as a new numpy array (int32)"""

    @property
    def scale_factors(self) -> Annotated[NDArray[numpy.float64], dict(shape=(None,))]:
        """
        The scale factors of the 24 BBOB functions, as a new numpy array (float64)
        """

    @property
    def sub_problems(self) -> list[BBOB]: ...

    @property
    def function_values(self) -> Annotated[NDArray[numpy.float64], dict(shape=(None,))]:
        """
        The values of the 24 BBOB functions, as a new numpy array (float64); evaluations do not set them, so they are zeros
        """

class DoubleFunnel(RealSingleObjective):
    @property
    def d(self) -> float: ...

    @property
    def s(self) -> float: ...

    @property
    def u1(self) -> float: ...

    @property
    def u2(self) -> float: ...

class DoubleSphere(DoubleFunnel):
    def __init__(self, n_variables: int, d: float = 0.0, s: float = 1.0) -> None: ...

class DoubleRastrigin(DoubleFunnel):
    def __init__(self, n_variables: int, d: float = 0.0, s: float = 1.0) -> None: ...
