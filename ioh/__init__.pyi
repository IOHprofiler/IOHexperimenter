import typing
from .iohcpp import IntegerConstraint as IntegerConstraint, IntegerSolution as IntegerSolution, IntegerState as IntegerState, LogInfo as LogInfo, MetaData as MetaData, OptimizationType as OptimizationType, RealConstraint as RealConstraint, RealSolution as RealSolution, RealState as RealState, logger, suite as suite
from typing import Any

ProblemType: Any

def get_problem(fid: typing.Union[int, str], iid: int, dim: int, problem_type: str = ...) -> ProblemType: ...
def wrap_problem(function) -> None: ...

class Experiment:
    algorithm: Any
    logger_root: Any
    logger_params: Any
    fids: Any
    iids: Any
    dims: Any
    reps: Any
    logged: Any
    problem_type: Any
    njobs: Any
    experiment_attributes: Any
    logged_attributes: Any
    run_attributes: Any
    merge_output: Any
    zip_output: Any
    remove_data: Any
    def __init__(self, algorithm: typing.Any, fids: typing.List[int], iids: typing.List[int], dims: typing.List[int], reps: int = ..., problem_type: str = ..., njobs: int = ..., logged: bool = ..., logger_triggers: typing.List[logger.trigger.Trigger] = ..., logger_additional_properties: typing.List[logger.property.AbstractProperty] = ..., output_directory: str = ..., folder_name: str = ..., algorithm_name: str = ..., algorithm_info: str = ..., store_positions: bool = ..., experiment_attributes: typing.Dict[str, str] = ..., run_attributes: typing.List[str] = ..., logged_attributes: typing.List[str] = ..., merge_output: bool = ..., zip_output: bool = ..., remove_data: bool = ...) -> None: ...
    def evaluate(self, ii: int, job: typing.Tuple[int, int, int]): ...
    def apply(self, algorithm: any, problem: ProblemType): ...
    def add_custom_problem(self, p: ProblemType, name: str = ..., **kwargs): ...
    def merge_output_to_single_folder(self, prefix: str, target_folder: str): ...
    def merge_tmp_folders(self) -> None: ...
    def run(self): ...
    def __call__(self): ...
