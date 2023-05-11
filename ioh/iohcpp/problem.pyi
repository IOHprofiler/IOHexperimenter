from typing import Any, ClassVar, List, Optional

import ioh.iohcpp as ic

class SingleObjective:
    def __init__(self, *args, **kwargs) -> None: ...
    def attach_logger(self, *args, **kwargs) -> Any: ...
    def create(self, *args, **kwargs) -> Any: ...
    def detach_logger(self, *args, **kwargs) -> Any: ...
    def reset(self, *args, **kwargs) -> Any: ...
    def __call__(self, *args, **kwargs) -> Any: ...
    def add_constraint(self, *args, **kwargs) -> None: ...
    def remove_constraint(self, *args, **kwargs) -> None: ...
    def enforce_bounds(self, weight:float, how: Any, exponent:float) -> None: ...

class AbstractWModel(IntegerSingleObjective):
    def __init__(self, problem_id: int, instance: int, n_variables: int, name: str, dummy_select_rate: float = ..., epistasis_block_size: int = ..., neutrality_mu: int = ..., ruggedness_gamma: int = ...) -> None: ...
    def wmodel_evaluate(self, arg0: List[int]) -> int: ...

class AttractiveSector(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class BBOB(RealSingleObjective):
    def __init__(self, *args, **kwargs) -> None: ...
    def create(self, *args, **kwargs) -> Any: ...

class BentCigar(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class BuecheRastrigin(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class ConcatenatedTrap(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class DifferentPowers(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Discus(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Ellipsoid(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class EllipsoidRotated(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Gallagher101(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Gallagher21(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class GraphProblem(IntegerSingleObjective):
    def __init__(self, *args, **kwargs) -> None: ...
    def create(self, *args, **kwargs) -> Any: ...

class GriewankRosenBrock(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class IntegerSingleObjective:
    problems: ClassVar[Any] = ...  # read-only
    def __init__(self, *args, **kwargs) -> None: ...
    def attach_logger(self, *args, **kwargs) -> Any: ...
    def create(self, *args, **kwargs) -> Any: ...
    def detach_logger(self, *args, **kwargs) -> Any: ...
    def reset(self, *args, **kwargs) -> Any: ...
    def __call__(self, *args, **kwargs) -> Any: ...
    def add_constraint(self, *args, **kwargs) -> None: ...
    def remove_constraint(self, *args, **kwargs) -> None: ...
    def enforce_bounds(self, weight:float, how: Any, exponent:float) -> None: ...
    @property
    def bounds(self) -> ic.IntegerBounds: ...
    @property
    def constraints(self) -> ic.IntegerConstraintSet: ...
    @property
    def log_info(self) -> ic.ic.LogInfo: ...
    @property
    def meta_data(self) -> ic.MetaData: ...
    @property
    def optimum(self) -> ic.IntegerSolution: ...
    @property
    def state(self) -> ic.IntegerState: ...
  

class IntegerWrappedProblem(IntegerSingleObjective):
    def __init__(self, *args, **kwargs) -> None: ...

class IsingRing(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class IsingTorus(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class IsingTriangular(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Katsuura(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class LABS(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnes(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnesDummy1(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnesDummy2(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnesEpistasis(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnesNeutrality(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnesRuggedness1(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnesRuggedness2(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class LeadingOnesRuggedness3(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Linear(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class LinearSlope(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class LunacekBiRastrigin(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class MIS(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class MaxCoverage(GraphProblem):
    def __init__(self, *args, **kwargs) -> None: ...
    def load_instances(self, *args, **kwargs) -> Any: ...

class MaxCut(GraphProblem):
    def __init__(self, *args, **kwargs) -> None: ...
    def load_instances(self, *args, **kwargs) -> Any: ...

class MaxInfluence(GraphProblem):
    def __init__(self, *args, **kwargs) -> None: ...
    def load_instances(self, *args, **kwargs) -> Any: ...

class NQueens(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMax(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMaxDummy1(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMaxDummy2(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMaxEpistasis(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMaxNeutrality(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMaxRuggedness1(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMaxRuggedness2(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class OneMaxRuggedness3(PBO):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class PBO(IntegerSingleObjective):
    def __init__(self, *args, **kwargs) -> None: ...
    def create(self, *args, **kwargs) -> Any: ...

class PackWhileTravel(GraphProblem):
    def __init__(self, *args, **kwargs) -> None: ...
    def load_instances(self, *args, **kwargs) -> Any: ...

class Rastrigin(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class RastriginRotated(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class RealSingleObjective:
    problems: ClassVar[Any] = ...  # read-only
    def __init__(self, *args, **kwargs) -> None: ...
    def attach_logger(self, *args, **kwargs) -> Any: ...
    def create(self, *args, **kwargs) -> Any: ...
    def detach_logger(self, *args, **kwargs) -> Any: ...
    def reset(self, *args, **kwargs) -> Any: ...
    def __call__(self, *args, **kwargs) -> Any: ...
    def add_constraint(self, *args, **kwargs) -> None: ...
    def remove_constraint(self, *args, **kwargs) -> None: ...
    def enforce_bounds(self, weight:float, how: Any, exponent:float) -> None: ...
    @property
    def bounds(self) -> ic.RealBounds: ...
    @property
    def constraints(self) -> ic.RealConstraintSet: ...
    @property
    def log_info(self) -> ic.ic.LogInfo: ...
    @property
    def meta_data(self) -> ic.MetaData: ...
    @property
    def optimum(self) -> ic.RealSolution: ...
    @property
    def state(self) -> ic.RealState: ...

class RealWrappedProblem(RealSingleObjective):
    def __init__(self, *args, **kwargs) -> None: ...

class Rosenbrock(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class RosenbrockRotated(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Schaffers10(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Schaffers1000(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Schwefel(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class SharpRidge(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class Sphere(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class StepEllipsoid(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

class WModelLeadingOnes(AbstractWModel):
    def __init__(self, instance: int, n_variables: int, dummy_select_rate: float = ..., epistasis_block_size: int = ..., neutrality_mu: int = ..., ruggedness_gamma: int = ...) -> None: ...

class WModelOneMax(AbstractWModel):
    def __init__(self, instance: int, n_variables: int, dummy_select_rate: float = ..., epistasis_block_size: int = ..., neutrality_mu: int = ..., ruggedness_gamma: int = ...) -> None: ...

class Weierstrass(BBOB):
    def __init__(self, instance: int, n_variables: int) -> None: ...

def wrap_integer_problem(f: callable, name: str, optimization_type: ic.OptimizationType = ..., lb: Optional[float] = ..., ub: Optional[float] = ..., transform_variables: Optional[callable] = ..., transform_objectives: Optional[callable] = ..., calculate_objective: Optional[callable] = ...) -> None: ...
def wrap_real_problem(f: callable, name: str, optimization_type: ic.OptimizationType = ..., lb: Optional[float] = ..., ub: Optional[float] = ..., transform_variables: Optional[callable] = ..., transform_objectives: Optional[callable] = ..., calculate_objective: Optional[callable] = ...) -> None: ...
