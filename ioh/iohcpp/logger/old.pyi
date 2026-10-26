from collections.abc import Mapping, Sequence
import os
from typing import overload

import ioh.iohcpp.logger
import ioh.iohcpp.logger.property
import ioh.iohcpp.logger.trigger


class Analyzer(ioh.iohcpp.logger.AbstractLogger):
    def __init__(self, triggers: Sequence[ioh.iohcpp.logger.trigger.Trigger] = ..., additional_properties: Sequence[ioh.iohcpp.logger.property.AbstractProperty] = [], root: str | os.PathLike = ..., folder_name: str = 'ioh_data', algorithm_name: str = 'algorithm_name', algorithm_info: str = 'algorithm_info', store_positions: bool = False) -> None:
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
    def watch(self, arg: ioh.iohcpp.logger.property.AbstractProperty, /) -> None: ...

    @overload
    def watch(self, arg0: object, arg1: str, /) -> None: ...

    @overload
    def watch(self, arg0: object, arg1: Sequence[str], /) -> None: ...

    def __repr__(self) -> str: ...
