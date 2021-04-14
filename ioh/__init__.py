'''Python specific functions for IOH package

TODO: check correct handling of object destructors
TODO: best far precision
TODO: tracking of out-of bounds
TODO: Check quadratic function, min dimension bbob=2
TODO: Rename Integer -> Discrete
TODO: Check what happens on exit -> probably factory objects need to be destroyed
'''

import os
import math
import itertools
import multiprocessing
import functools
import typing
import copy
import atexit

try:
    from .iohcpp import *
except ModuleNotFoundError:
    raise ModuleNotFoundError("No module named ioh")


atexit.register(functools.partial(os._exit, 0))

def get_problem(fid: int, iid: int, dim: int, problem_type: str = "Real"):
    '''Instantiate a problem based on its function ID, dimension, instance and suite

    Parameters
    ----------
    fid: int or str
        The function ID of the problem in the suite, or the name of the function as string
    dim: int
        The dimension (number of variables) of the problem
    iid: int
        The instance ID of the problem
    problem_type: str
        Which suite the problem is from. Either 'BBOB' or 'PBO' or 'Real' or 'Integer'
        Only used if fid is an int.

    '''
    if isinstance(fid, str):
        try:
            return getattr(problem, fid)(iid, dim)
        except:
            raise ValueError(f"Unkown problem {fid} is given")

    if problem_type in ("BBOB", "Real",):
        return getattr(problem, problem_type).factory().create(fid, iid, dim)
    elif problem_type in ("PBO", "Integer",):
        if fid in [21, 23]:
            if not math.sqrt(dim).is_integer():
                raise ValueError("For this function, the dimension needs to be a perfect square!")
        return getattr(problem, problem_type).factory().create(fid, iid, dim)

    raise ValueError(f"Suite {problem_type} is not yet supported")    

class Experiment:
    def __init__(self, 
        algorithm: typing.Any,
        fids: typing.List[int], 
        iids: typing.List[int], 
        dims: typing.List[int], 
        reps: int = 1, 
        problem_type: str = "BBOB", 
        njobs: int = 1, 
        logged: bool = True,
        output_directory: str = "./",
        folder_name: str = "ioh_data",
        algorithm_name: str = "algorithm_name",
        algorithm_info: str = "algorithm_info",
        optimization_type: OptimizationType = OptimizationType.Minimization,
        store_positions: bool = False,
        t_always: bool = False,
        t_on_interval: int = 0,
        t_per_time_range: int = 0,
        t_on_improvement: bool = True,
        t_at_time_points: typing.List[int] = [],
        trigger_at_time_points_exp_base: int = 10,
        trigger_at_range_exp_base: int = 10,
        experiment_attributes: typing.List[typing.Tuple[str, float]] = [],
        run_attributes: typing.List[str] = [],
        logged_attributes: typing.List[str] = []
    ): 
        self.logger_params = dict(
            output_directory = output_directory,
            folder_name = folder_name,
            algorithm_name = algorithm_name,
            algorithm_info = algorithm_info,
            optimization_type = optimization_type,
            store_positions = store_positions,
            t_always = t_always,
            t_on_interval = t_on_interval,
            t_per_time_range = t_per_time_range,
            t_on_improvement = t_on_improvement,
            t_at_time_points = t_at_time_points,
            trigger_at_time_points_exp_base = trigger_at_time_points_exp_base,
            trigger_at_range_exp_base = trigger_at_range_exp_base,
        )
        self.algorithm = algorithm
        self.fids = fids
        self.iids = iids 
        self.dims = dims 
        self.reps = reps
        self.logged = logged
        self.problem_type = problem_type
        self.njobs = njobs if njobs != -1 else multiprocessing.cpu_count()
        self.experiment_attributes = experiment_attributes
        self.run_attributes = run_attributes
        self.logged_attributes = logged_attributes
        
        for attr in itertools.chain(self.run_attributes, self.logged_attributes):
            if not hasattr(self.algorithm, attr):
                raise TypeError(
                    f"Attribute {attr} is a member of algorithm {self.algorithm}"
                )


    def evaluate(self, fid: int, iid: int, dim: int):
        '''Evaluate a single function using self.algoritm.

        Note that this functions makes a copy of the algorithm for each new problem
        instantiation.
        
        Parameters
        ----------
        fid: int
            The problem id
        iid: int
            The instance id
        dim: int
            The problem dimension
        '''

        algorithm = copy.deepcopy(self.algorithm)
        p = get_problem(fid, iid, dim, self.problem_type)
        if self.logged:
            l = logger.Default(**self.logger_params)

            l.declare_experiment_attributes(*zip(*self.experiment_attributes))
            l.declare_run_attributes(algorithm, self.run_attributes)
            l.declare_logged_attributes(algorithm, self.logged_attributes)
            p.attach_logger(l)
            
        for i in range(self.reps):
            algorithm(p) 
            p.reset()
        
        
    def add_custom_problem(self, p: typing.Callable, name: str = None):
        '''Add a custom problem to the list of functions to be evaluated.

        Parameters
        ---------
        p: callable
            A function which takes an list of numeric values and return as list
            of numeric values
        
        name: str
            An optional name of the the newly added function.    
        '''
        if self.problem_type in ("PBO", "Integer"):
            p = problem.wrap_integer_problem(p, name or "CustomProblem")
            self.problem_type = "Integer"
        else:
            p = problem.wrap_real_problem(p, name or "CustomProblem")
            self.problem_type = "Real"

        self.fids.append(p.meta_data.problem_id)

    def __call__(self):
        '''Run the experiment'''

        iterator = itertools.product(self.fids, self.iids, self.dims)
        if self.njobs != 1:
            with multiprocessing.Pool(self.njobs) as p:
                p.starmap(self.evaluate, iterator)
        else:
            for fid, iid, dim in iterator:
                self.evaluate(fid, iid, dim)

