"""Module that includes all IOHExperimenter suites"""

from collections.abc import Iterator, Sequence
from typing import Annotated

import numpy
from numpy.typing import NDArray

import ioh.iohcpp.logger
import ioh.iohcpp.problem


class RealBase:
    """RealBase suite"""

    def reset(self) -> None:
        """Reset the state variables of the current problem."""

    def attach_logger(self, arg: ioh.iohcpp.logger.Logger, /) -> None:
        """
        Attach a logger to the problem to track the performance and internal variables.
        """

    def detach_logger(self) -> None:
        """Remove the specified logger from the problem"""

    @property
    def problem_ids(self) -> Annotated[NDArray[numpy.int32], dict(shape=(None,))]:
        """
        The ids of all problems contained in the current suite, as a new numpy array (int32).
        """

    @property
    def dimensions(self) -> Annotated[NDArray[numpy.int32], dict(shape=(None,))]:
        """
        The dimensions of all problems contained in the current suite, as a new numpy array (int32).
        """

    @property
    def instances(self) -> Annotated[NDArray[numpy.int32], dict(shape=(None,))]:
        """
        The instance ids of all problems contained in the current suite, as a new numpy array (int32).
        """

    @property
    def name(self) -> str:
        """The name of the suite."""

    def __len__(self) -> int: ...

    def __iter__(self) -> Iterator[ioh.iohcpp.problem.RealSingleObjective]: ...

class Real(RealBase):
    """
    Suite with Real functions included in IOH. 

    This class is an iterator for problems, you can define a set of problems to iterate over.

    Parameters
    ----------
    problem_ids: list[int]
        A list of problems ids to include in this instantiation of the suite
    instances: list[int]
        A list of problem instances to include in this instantiation of the suite
    dimensions: list[int]
        A list of problem dimensions to include in this instantiation of the suite
    """

    def __init__(self, problem_ids: Sequence[int], instances: Sequence[int] = [1], dimensions: Sequence[int] = [5]) -> None: ...

class BBOB(RealBase):
    """
    Suite with BBOB functions included in IOH. 

    This class is an iterator for problems, you can define a set of problems to iterate over.

    Parameters
    ----------
    problem_ids: list[int]
        A list of problems ids to include in this instantiation of the suite
    instances: list[int]
        A list of problem instances to include in this instantiation of the suite
    dimensions: list[int]
        A list of problem dimensions to include in this instantiation of the suite
    """

    def __init__(self, problem_ids: Sequence[int], instances: Sequence[int] = [1], dimensions: Sequence[int] = [5]) -> None: ...

class SBOX(RealBase):
    """
    Suite with SBOX functions included in IOH. 

    This class is an iterator for problems, you can define a set of problems to iterate over.

    Parameters
    ----------
    problem_ids: list[int]
        A list of problems ids to include in this instantiation of the suite
    instances: list[int]
        A list of problem instances to include in this instantiation of the suite
    dimensions: list[int]
        A list of problem dimensions to include in this instantiation of the suite
    """

    def __init__(self, problem_ids: Sequence[int], instances: Sequence[int] = [1], dimensions: Sequence[int] = [5]) -> None: ...

class RealStarDiscrepancy(RealBase):
    """
    Suite with RealStarDiscrepancy functions included in IOH. 

    This class is an iterator for problems, you can define a set of problems to iterate over.

    Parameters
    ----------
    problem_ids: list[int]
        A list of problems ids to include in this instantiation of the suite
    instances: list[int]
        A list of problem instances to include in this instantiation of the suite
    dimensions: list[int]
        A list of problem dimensions to include in this instantiation of the suite
    """

    def __init__(self, problem_ids: Sequence[int], instances: Sequence[int] = [1], dimensions: Sequence[int] = [5]) -> None: ...

class IntegerBase:
    """IntegerBase suite"""

    def reset(self) -> None:
        """Reset the state variables of the current problem."""

    def attach_logger(self, arg: ioh.iohcpp.logger.Logger, /) -> None:
        """
        Attach a logger to the problem to track the performance and internal variables.
        """

    def detach_logger(self) -> None:
        """Remove the specified logger from the problem"""

    @property
    def problem_ids(self) -> Annotated[NDArray[numpy.int32], dict(shape=(None,))]:
        """
        The ids of all problems contained in the current suite, as a new numpy array (int32).
        """

    @property
    def dimensions(self) -> Annotated[NDArray[numpy.int32], dict(shape=(None,))]:
        """
        The dimensions of all problems contained in the current suite, as a new numpy array (int32).
        """

    @property
    def instances(self) -> Annotated[NDArray[numpy.int32], dict(shape=(None,))]:
        """
        The instance ids of all problems contained in the current suite, as a new numpy array (int32).
        """

    @property
    def name(self) -> str:
        """The name of the suite."""

    def __len__(self) -> int: ...

    def __iter__(self) -> Iterator[ioh.iohcpp.problem.IntegerSingleObjective]: ...

class Integer(IntegerBase):
    """
    Suite with Integer functions included in IOH. 

    This class is an iterator for problems, you can define a set of problems to iterate over.

    Parameters
    ----------
    problem_ids: list[int]
        A list of problems ids to include in this instantiation of the suite
    instances: list[int]
        A list of problem instances to include in this instantiation of the suite
    dimensions: list[int]
        A list of problem dimensions to include in this instantiation of the suite
    """

    def __init__(self, problem_ids: Sequence[int], instances: Sequence[int] = [1], dimensions: Sequence[int] = [5]) -> None: ...

class PBO(IntegerBase):
    """
    Suite with PBO functions included in IOH. 

    This class is an iterator for problems, you can define a set of problems to iterate over.

    Parameters
    ----------
    problem_ids: list[int]
        A list of problems ids to include in this instantiation of the suite
    instances: list[int]
        A list of problem instances to include in this instantiation of the suite
    dimensions: list[int]
        A list of problem dimensions to include in this instantiation of the suite
    """

    def __init__(self, problem_ids: Sequence[int], instances: Sequence[int] = [1], dimensions: Sequence[int] = [16]) -> None: ...

class Submodular(IntegerBase):
    """
    Suite with Submodular functions included in IOH. 

    This class is an iterator for problems, you can define a set of problems to iterate over.

    Parameters
    ----------
    problem_ids: list[int]
        A list of problems ids to include in this instantiation of the suite
    instances: list[int]
        A list of problem instances to include in this instantiation of the suite
    dimensions: list[int]
        A list of problem dimensions to include in this instantiation of the suite
    """

    def __init__(self, problem_ids: Sequence[int], instances: Sequence[int] = [1], dimensions: Sequence[int] = [1]) -> None: ...

class IntegerStarDiscrepancy(IntegerBase):
    """
    Suite with IntegerStarDiscrepancy functions included in IOH. 

    This class is an iterator for problems, you can define a set of problems to iterate over.

    Parameters
    ----------
    problem_ids: list[int]
        A list of problems ids to include in this instantiation of the suite
    instances: list[int]
        A list of problem instances to include in this instantiation of the suite
    dimensions: list[int]
        A list of problem dimensions to include in this instantiation of the suite
    """

    def __init__(self, problem_ids: Sequence[int], instances: Sequence[int] = [1], dimensions: Sequence[int] = [5]) -> None: ...
