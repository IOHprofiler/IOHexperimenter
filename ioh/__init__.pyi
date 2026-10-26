import enum
import typing
from .iohcpp import DataError as DataError, IntegerConstraint, IntegerSolution, OptimizationType, RealConstraint, RealSolution, data_root as data_root, logger, problem
from _typeshed import Incomplete

__all__ = ['get_problem', 'wrap_problem', 'get_problem_id', 'Experiment', 'set_data_root', 'data_root', 'DataError']

def set_data_root(path) -> None: ...
ProblemType = problem.RealSingleObjective | problem.IntegerSingleObjective
VariableType = int | float
ObjectiveType = list[VariableType]

class ProblemClass(enum.Enum):
    REAL = 'RealSingleObjective'
    INTEGER = 'IntegerSingleObjective'
    BBOB = 'BBOB'
    STAR_REAL = 'RealStarDiscrepancy'
    SBOX = 'SBOX'
    PBO = 'PBO'
    GRAPH = 'GraphProblem'
    STAR_INTEGER = 'IntegerStarDiscrepancy'
    CEC2013 = 'CEC2013'
    CEC2022 = 'CEC2022'
    def is_real(self): ...
    def is_single_objective(self): ...
    @property
    def problems(self): ...

def get_problem(fid: int | str, instance: int = 1, dimension: int = 5, problem_class: ProblemClass = ...) -> ProblemType: ...
def wrap_problem(function: typing.Callable[[ObjectiveType], float], name: str = None, problem_class: ProblemClass = ..., dimension: int = 5, instance: int = 1, optimization_type: OptimizationType = ..., lb: VariableType = 0, ub: VariableType = 1, transform_variables: typing.Callable[[ObjectiveType, int], ObjectiveType] = None, transform_objectives: typing.Callable[[float, int], float] = None, calculate_objective: typing.Callable[[int, int], IntegerSolution | RealSolution] = None, constraints: list[IntegerConstraint | RealConstraint] = None) -> ProblemType: ...
def get_problem_id(problem_name: str, problem_class: ProblemClass) -> int: ...

class Experiment:
    algorithm: Incomplete
    logger_root: Incomplete
    logger_params: Incomplete
    fids: Incomplete
    iids: Incomplete
    dims: Incomplete
    reps: Incomplete
    logged: Incomplete
    problem_class: Incomplete
    njobs: Incomplete
    experiment_attributes: Incomplete
    logged_attributes: Incomplete
    run_attributes: Incomplete
    merge_output: Incomplete
    zip_output: Incomplete
    remove_data: Incomplete
    enforce_bounds: Incomplete
    old_logger: Incomplete
    def __init__(self, algorithm: typing.Any, fids: list[int], iids: list[int], dims: list[int], reps: int = 1, problem_class: ProblemClass = ..., njobs: int = 1, logged: bool = True, logger_triggers: list[logger.trigger.Trigger] = ..., logger_additional_properties: list[logger.property.AbstractProperty] = [], output_directory: str = './', folder_name: str = 'ioh_data', algorithm_name: str = None, algorithm_info: str = 'algorithm_info', store_positions: bool = False, experiment_attributes: dict[str, str] = {}, run_attributes: list[str] = [], logged_attributes: list[str] = [], merge_output: bool = True, zip_output: bool = True, remove_data: bool = False, enforce_bounds: bool = False, old_logger: bool = False) -> None: ...
    def evaluate(self, ii: int, job: tuple[int, int, int]): ...
    def apply(self, algorithm: any, problem: ProblemType): ...
    def add_custom_problem(self, p: callable, name: str = None, **kwargs): ...
    def merge_output_to_single_folder(self, prefix: str, target_folder: str): ...
    def merge_tmp_folders(self) -> None: ...
    def run(self): ...
    def __call__(self): ...
