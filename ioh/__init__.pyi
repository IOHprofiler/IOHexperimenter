import typing
import enum
from .iohcpp import IntegerSolution, OptimizationType, RealSolution

from .iohcpp import problem as problem
from .iohcpp import suite as suite
from .iohcpp import logger as logger

from typing import Any

ProblemType = typing.Union[problem.Real, problem.Integer]
VariableType = typing.Union[int, float]
ObjectiveType = typing.List[VariableType]

class ProblemType(enum.Enum):
    """Enum for different problem types, values are class names"""

    REAL = "RealSingleObjective"
    INTEGER = "IntegerSingleObjective"

    BBOB = "BBOB"
    PBO = "PBO"
    GRAPH = "GraphProblem"

    def is_real(self):
        ...

    def is_single_objective(self): 
        ...

def get_problem(fid: typing.Union[int, str], instance: int = ..., dimension: int = ..., problem_type: ProblemType = ...) -> ProblemType: ...
def wrap_problem(function: typing.Callable[[ObjectiveType], float], name: str, problem_type: ProblemType, dimension: int = ..., instance: int = ..., optimization_type: OptimizationType = ..., lb: VariableType = ..., ub: VariableType = ..., transform_variables: typing.Callable[[ObjectiveType, int], ObjectiveType] = ..., transform_objectives: typing.Callable[[float, int], float] = ..., calculate_objective: typing.Callable[[int, int], typing.Union[IntegerSolution, RealSolution]] = ...) -> ProblemType: ...
def get_problem_id(problem_name: str, problem_type: ProblemType) -> int: ...

class Experiment:
    algorithm: Any
    logger_root: str
    logger_params: Any
    fids: typing.List[int]
    iids: typing.List[int]
    dims: typing.List[int]
    reps: int
    logged: bool
    problem_type: ProblemType
    njobs: int
    experiment_attributes: typing.Dict
    logged_attributes: Any
    run_attributes: Any
    merge_output: bool
    zip_output: bool
    remove_data: bool
    def __init__(self, algorithm: typing.Any, fids: typing.List[int], iids: typing.List[int], dims: typing.List[int], reps: int = ..., problem_type: str = ..., njobs: int = ..., logged: bool = ..., logger_triggers: typing.List[logger.trigger.Trigger] = ..., logger_additional_properties: typing.List[logger.property.AbstractProperty] = ..., output_directory: str = ..., folder_name: str = ..., algorithm_name: str = ..., algorithm_info: str = ..., store_positions: bool = ..., experiment_attributes: typing.Dict[str, str] = ..., run_attributes: typing.List[str] = ..., logged_attributes: typing.List[str] = ..., merge_output: bool = ..., zip_output: bool = ..., remove_data: bool = ...) -> None: ...
    def evaluate(self, ii: int, job: typing.Tuple[int, int, int]): ...
    def apply(self, algorithm: any, problem: ProblemType): ...
    def add_custom_problem(self, p: ProblemType, name: str = ..., **kwargs): ...
    def merge_output_to_single_folder(self, prefix: str, target_folder: str): ...
    def merge_tmp_folders(self) -> None: ...
    def run(self): ...
    def __call__(self): ...
