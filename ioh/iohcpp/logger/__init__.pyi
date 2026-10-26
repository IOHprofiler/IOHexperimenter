"""Module that includes all IOHExperimenter loggers"""

from collections.abc import Mapping, Sequence
import os
from typing import overload

import ioh.iohcpp
from ioh.iohcpp.logger import (
    eaf as eaf,
    eah as eah,
    old as old,
    property as property,
    trigger as trigger
)


class Logger:
    """Base class for all loggers"""

    def add_trigger(self, arg: trigger.Trigger, /) -> None:
        """Add a trigger to the logger"""

    def call(self, arg: ioh.iohcpp.LogInfo, /) -> None:
        """Performs logging behaviour"""

    def reset(self) -> None:
        """Reset the state of the logger"""

    @property
    def problem(self) -> ioh.iohcpp.MetaData | None:
        """Reference to the currently attached problem"""

class AbstractLogger(Logger):
    """Base class for loggers which track properties"""

    def __init__(self, triggers: Sequence[trigger.Trigger] = [], properties: Sequence[property.AbstractProperty] = []) -> None: ...

    def watch(self, arg: property.AbstractProperty, /) -> None: ...

    def attach_problem(self, problem: ioh.iohcpp.MetaData) -> None:
        """attach a problem (MetaData) to a logger"""

class Combine(Logger):
    """Utility class to combine multiple loggers"""

    @overload
    def __init__(self, loggers: Sequence[Logger]) -> None: ...

    @overload
    def __init__(self, logger: Logger) -> None: ...

    def append(self, arg: Logger, /) -> None: ...

class FlatFile(AbstractLogger):
    def __init__(self, triggers: Sequence[trigger.Trigger], properties: Sequence[property.AbstractProperty], filename: str = 'IOH.dat', output_directory: str | os.PathLike = './', separator: str = '\t', comment: str = '#', no_value: str = 'None', end_of_line: str = '\n', repeat_header: bool = False, store_positions: bool = False, common_header_titles: Sequence[str] = ...) -> None:
        """
        A logger which stores all tracked properties to a file.

        Parameters
        ----------
        triggers: list[Trigger]
            List of triggers, i.e. when to trigger logging
        properties: list[Property]
            The list of properties to keep track of
        filename: str = "IOH.dat"
            The file to log to
        output_directory: str = "./"
            The path to which to write the file
        separator: str = "\\t"
            The column seperator
        comment: str = "#"
            The comment character
        no_value: str = "None"
            The NaN character
        end_of_line: str = "\\n"
            The eol character
        repeat_header: bool = false
            Boolean value which indicates whether to repeat the header for every new run.
        store_positions: bool = false
            Boolean value which indicates whether to store the x-positions in the file
        """

    @property
    def filename(self) -> str: ...

    @property
    def output_directory(self) -> str: ...

    @overload
    def watch(self, arg: property.AbstractProperty, /) -> None: ...

    @overload
    def watch(self, arg0: object, arg1: str, /) -> None: ...

    @overload
    def watch(self, arg0: object, arg1: Sequence[str], /) -> None: ...

    def __repr__(self) -> str: ...

class Store(AbstractLogger):
    def __init__(self, triggers: Sequence[trigger.Trigger], properties: Sequence[property.AbstractProperty]) -> None:
        """
        A logger which stores all tracked properties in memory.

        Parameters
        ----------
        triggers: list[Trigger]
            List of triggers, i.e. when to trigger logging
        properties: list[Property]
            The list of properties to keep track of
        """

    def data(self) -> dict[str, dict[int, dict[int, dict[int, dict[int, dict[int, dict[str, float | None]]]]]]]:
        """Accessor to the internal data container"""

    def at(self, arg0: str, arg1: int, arg2: int, arg3: int, arg4: int, arg5: int, /) -> dict[str, float | None]:
        """Accessor for a specific stored record"""

    @overload
    def watch(self, arg: property.AbstractProperty, /) -> None: ...

    @overload
    def watch(self, arg0: object, arg1: str, /) -> None:
        """Add a property"""

    @overload
    def watch(self, arg0: object, arg1: Sequence[str], /) -> None:
        """Add multliple properties"""

    def __repr__(self) -> str: ...

class Analyzer(AbstractLogger):
    def __init__(self, triggers: Sequence[trigger.Trigger] = ..., additional_properties: Sequence[property.AbstractProperty] = [], root: str | os.PathLike = ..., folder_name: str = 'ioh_data', algorithm_name: str = 'algorithm_name', algorithm_info: str = 'algorithm_info', store_positions: bool = False) -> None:
        """
        A logger which stores all tracked properties to a file.

        Parameters
        ----------
        triggers: list[Trigger]
            List of triggers, i.e. when to trigger logging
        additional_properties: list[Property]
            The list of additional properties to keep track of (additional to the default properties for this logger type)
        root: str = "./"
            The path to which to write the files
        folder_name: str = "./"
            The name of the folder to which to write the files
        algorithm_name: str = "algorithm_name"
            Optional name parameter for the algorithm to be added to the info files
        algorithm_info: str = "algorithm_info"
            Optional info parameter for the algorithm to be added to the info files
        store_positions: bool = false
            Boolean value which indicates whether to store the x-positions in the file
        """

    def add_experiment_attribute(self, arg0: str, arg1: str, /) -> None: ...

    def set_experiment_attributes(self, arg: Mapping[str, str], /) -> None: ...

    @overload
    def add_run_attribute(self, arg0: str, arg1: float, /) -> None: ...

    @overload
    def add_run_attribute(self, arg0: object, arg1: str, /) -> None: ...

    def add_run_attributes(self, arg0: object, arg1: Sequence[str], /) -> None: ...

    def set_run_attributes(self, arg: Mapping[str, float], /) -> None: ...

    def set_run_attribute(self, arg0: str, arg1: float, /) -> None: ...

    @property
    def output_directory(self) -> str: ...

    def close(self) -> None: ...

    @overload
    def watch(self, arg: property.AbstractProperty, /) -> None: ...

    @overload
    def watch(self, arg0: object, arg1: str, /) -> None: ...

    @overload
    def watch(self, arg0: object, arg1: Sequence[str], /) -> None: ...

    def __repr__(self) -> str: ...

class EAH(Logger):
    """Emperical Attainment Histogram Logger"""

    @overload
    def __init__(self, error_min: float, error_max: float, error_buckets: int, evals_min: int, evals_max: int, evals_buckets: int) -> None: ...

    @overload
    def __init__(self, error_scale: eah.LinearRealScale, eval_scale: eah.LinearIntegerScale) -> None: ...

    @overload
    def __init__(self, error_scale: eah.Log2RealScale, eval_scale: eah.Log2IntegerScale) -> None: ...

    @overload
    def __init__(self, error_scale: eah.Log10RealScale, eval_scale: eah.Log10IntegerScale) -> None: ...

    def at(self, arg0: int, arg1: int, arg2: int, arg3: int, /) -> list[list[bool]]: ...

    @property
    def data(self) -> dict[int, dict[int, dict[int, dict[int, list[list[bool]]]]]]: ...

    @property
    def size(self) -> tuple[int, int, int, int]: ...

    @property
    def error_range(self) -> eah.RealScale: ...

    @property
    def eval_range(self) -> eah.IntegerScale: ...

    def __repr__(self) -> str: ...

class EAF(Logger):
    """Emperical Attainment Function Logger"""

    def __init__(self) -> None: ...

    @property
    def data(self) -> dict[str, dict[int, dict[int, dict[int, dict[int, list[eaf.RunPoint]]]]]]: ...

    def at(self, arg0: str, arg1: int, arg2: int, arg3: int, arg4: int, /) -> list[eaf.RunPoint]: ...

from pathlib import Path as Path
