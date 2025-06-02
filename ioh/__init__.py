'''Python interface of IOH package. Includes several ease-of-use routines not available in C++.'''

import os
import enum
import math
import warnings
import itertools
import multiprocessing
import typing
import shutil
import copy
import json

import urllib.request
import tarfile

# Set the path to the static/ directory.
# NEEDED for C++ code to load transformation details for several functions.
# The static/ directory is installed together with the "pip install" command.
# WARNING: The os.environ should be manipulated before any package from .iohcpp is imported!
# =================================================================================================
# Get the directory containing the __init__.py file
package_directory = os.path.realpath(os.path.dirname(__file__))

# Construct the path to the static/ directory
static_directory = os.path.join(package_directory, 'static')

# Set the IOH_RESOURCES environment variable to the path to the static/ directory
if os.environ.get('IOH_RESOURCES') is None:
    os.environ['IOH_RESOURCES'] = static_directory
    
# Raise informative error
if not os.path.isdir(os.environ['IOH_RESOURCES']):
    raise ImportError(
        f"static directory not found. Please manually set the environment variable IOH_RESOURCES "
        f"to the path of the unpacked static.tar.gz (see: "
        f"https://github.com/IOHprofiler/IOHexperimenter/blob/master/static.tar.gz)"
    )        
    
# =================================================================================================

from .iohcpp import (
    problem,
    suite, 
    logger,
    OptimizationType,
    RealSolution,
    IntegerSolution,
    MultiRealSolution,
    MultiIntegerSolution,
    IntegerBounds,
    RealBounds,
    IntegerConstraint,
    RealConstraint,
    ConstraintEnforcement,
    RealState, 
    IntegerState,
    MetaData,
    LogInfo,
)

ProblemType = typing.Union[problem.RealSingleObjective, problem.IntegerSingleObjective]
VariableType = typing.Union[int, float]
ObjectiveType = typing.List[VariableType]


def download_static_folder(warn = True):
    '''Download static files from github. 

    Required for Graph problems to be instantiated properly
    '''

    target = os.path.realpath("IOHexperimenter")
    branch = "master"

    if os.path.isdir(target) and warn:
        warnings.warn(f"Attempting to download static folder but path {target} already exists. Skipping...")
        return 
    
    os.makedirs(target)
    github_static_folder = f"https://github.com/IOHprofiler/IOHexperimenter/blob/{branch}/static.tar.gz?raw=true" 

    with urllib.request.urlopen(github_static_folder) as f:
        thetarfile = tarfile.open(fileobj=f, mode="r|gz")
        thetarfile.extractall(target)


def load_graph_problems():
    '''Helper to load Graph problems. '''

    download_static_folder(False)

    for problem_class in (
        problem.MaxCut,
        problem.MaxCoverage,
        problem.MaxInfluence,
        problem.PackWhileTravel
    ):
        problem_class.load_instances()

    assert any(problem.GraphProblem.problems), "loading the graph files failed"


class ProblemClass(enum.Enum):
    '''Enum for different problem types, values are class names.
    '''

    REAL = "RealSingleObjective"
    INTEGER = "IntegerSingleObjective"

    MULTI_REAL = "RealMultiObjective"
    MULTI_INTEGER = "IntegerMultiObjective"

    BBOB = "BBOB"
    STAR_REAL = "RealStarDiscrepancy"
    SBOX = "SBOX"

    PBO = "PBO"
    GRAPH = "GraphProblem"
    STAR_INTEGER = "IntegerStarDiscrepancy"

    CEC2013 = "CEC2013"
    CEC2022 = "CEC2022"

    def is_real(self):
        return self in (
                ProblemClass.CEC2013,
                ProblemClass.CEC2022,
                ProblemClass.REAL, 
                ProblemClass.BBOB,
                ProblemClass.SBOX,
                ProblemClass.STAR_REAL,
                ) 
    
    def is_single_objective(self):
        if(self in (
            ProblemClass.MULTI_REAL,
            ProblemClass.MULTI_INTEGER,
            )):
            return False

        return True

    @property
    def problems(self):
        base_problem = getattr(problem, self.value)
        if base_problem:
            if self is ProblemClass.GRAPH and not any(base_problem.problems):
                load_graph_problems()
            return base_problem.problems


def get_problem(
    fid: typing.Union[int, str],
    instance: int = 1,
    dimension: int = 5,
    problem_class: ProblemClass = ProblemClass.REAL,
) -> ProblemType:
    '''Instantiate a problem based on its function ID, dimension, instance and suite

    Parameters
    ----------
    fid: int or str
        The function ID of the problem in the suite, or the name of the function as string
    instance: int = 1
        The instance ID of the problem
    dimension: int = 5
        The dimension (number of variables) of the problem
    problem_class: ProblemClass = ProblemClass.REAL
        The type of the problem.

    '''
    if not isinstance(problem_class, ProblemClass):
        raise AttributeError(f"problem_class should be of type {ProblemClass}")
    if(problem_class.is_single_objective()):
        if ( 
            not problem_class.is_real()
            and fid in [21, 23, "IsingTriangular", "NQueens"]
        ):
            if not math.sqrt(dimension).is_integer():
                raise ValueError(
                    "For this function, the dimension needs to be a perfect square!"
                )
        if (
            problem_class.is_real()
            and fid in range(1, 25)
        ):
            if not dimension >= 2:
                raise ValueError("For BBOB functions the minimal dimension is 2")

    base_problem = getattr(problem, problem_class.value)
    if base_problem:
        if problem_class is ProblemClass.GRAPH and not any(base_problem.problems):
            load_graph_problems()

        if fid not in (base_problem.problems.values() | base_problem.problems.keys()):
            raise ValueError(
                f"{fid} is not registered for problem type: {problem_class}"
            )
        return base_problem.create(fid, instance, dimension)

    raise ValueError(f"Problem type {problem_class} is not supported")


def wrap_problem(
    function: typing.Callable[[ObjectiveType], float],
    name: str = None,
    problem_class: ProblemClass = ProblemClass.REAL,
    dimension: int = 5,
    instance: int = 1,
    optimization_type: OptimizationType = OptimizationType.MIN,
    lb: VariableType = 0,
    ub: VariableType = 1,
    transform_variables: typing.Callable[[ObjectiveType, int], ObjectiveType] = None,
    transform_objectives: typing.Callable[[float, int], float] = None,
    calculate_objective: typing.Callable[
        [int, int], typing.Union[IntegerSolution, RealSolution]
    ] = None,
    constraints: typing.List[typing.Union[IntegerConstraint, RealConstraint]] = None

) -> ProblemType:
    '''Function to wrap a callable as an ioh function

    Parameters
    ----------
    function: fn(x: list) -> float
        The callable to wrap
    name: str = None
        The name of the function. This can be used to create new instances of this function.
        Note, when not using unique names, this will override the previously wrapped functions.
    problem_class: ProblemClass = ProblemClass.REAL
        The type of the problem.
    dimension: int = 5
        The dimension (number of variables) of the problem
    instance: int = 1
        The instance ID of the problem
    optimization_type: OptimizationType = OptimizationType.MIN
        The type of optimization to do, maximization or minimization
    lb: [int, float] = 0
        The lower bound of the constraint, should be the same type as problem_class. Defaults to 0
    ub: [int, float] = 1
        The upper bound of the constraint, should be the same type as problem_class. Defaults to 1
    transform_variables: fn(x: list) -> list = None
        A function to transform the elements of x, prior to calling the function.
    transform_objectives: fn(y: float) -> float = None
        A function to tranform the float value of y, after callling the function.
    calculate_objective: fn(instance, dimension) -> Solution | (x, y) = None
        A function to calculate the global optimum of the function. This function gets a dimension and instance id,
        and should return either a Solution objective(IntegerSolution or RealSolution) or a tuple giving the
        x and y values for the global optimum. Where x is the search space representation and y the target value.
    constraints: list[IntegerConstraint | RealConstraint] = None
        The constraints applied to the problem
    '''
    if not isinstance(problem_class, ProblemClass):
        raise AttributeError(f"problem_class should be of type {ProblemClass}")

    if name is None:
        name = function.__name__
    
    if not problem_class.is_single_objective():
        raise ValueError(
            f"Problem type {problem_class} is not supported."
        )

    wrapper = problem.wrap_real_problem if problem_class.is_real() \
        else problem.wrap_integer_problem 

    wrapper(
        function,
        name,
        optimization_type,
        lb,
        ub,
        transform_variables,
        transform_objectives,
        calculate_objective,
        [] if constraints is None else constraints
    )
    return get_problem(name, instance, dimension, problem_class)


def get_problem_id(problem_name: str, problem_class: ProblemClass) -> int: 
    '''Get the problem id corresponding to a problem name of a given type
    
    Parameters
    ----------
    problem_name: str
        The name of the problem
    problem_class: ProblemClass
        The type of the problem
    
    Returns
    -------
    int: the id of the problem
    '''    
    if not isinstance(problem_class, ProblemClass):
        raise AttributeError(f"problem_class should be of type {ProblemClass}")

    return {v: k for k, v in getattr(problem, problem_class.value).problems.items()}.get(
        problem_name
    )


class Experiment:
    '''Class to help easily setup benchmarking experiments. '''

    def __init__(
        self,
        algorithm: typing.Any,
        fids: typing.List[int],
        iids: typing.List[int],
        dims: typing.List[int],
        reps: int = 1,
        problem_class: ProblemClass = ProblemClass.REAL,
        njobs: int = 1,
        logged: bool = True,
        logger_triggers: typing.List[logger.trigger.Trigger] = [
            logger.trigger.ON_IMPROVEMENT
        ],
        logger_additional_properties: typing.List[
            logger.property.AbstractProperty
        ] = [],
        output_directory: str = "./",
        folder_name: str = "ioh_data",
        algorithm_name: str = None,
        algorithm_info: str = "algorithm_info",
        store_positions: bool = False,
        experiment_attributes: typing.Dict[str, str] = {},
        run_attributes: typing.List[str] = [],
        logged_attributes: typing.List[str] = [],
        merge_output: bool = True,
        zip_output: bool = True,
        remove_data: bool = False,
        enforce_bounds: bool = False,
        old_logger: bool = False
    ):
        '''
        Parameters
        ----------
            algorithm: typing.Any
                A instance of a given algorithm. Must implement a __call__ method
                taking a ioh.problem as argument.
            fids: typing.List[int]
                A list of integer problem ids to evaluate
            iids: typing.List[int],
                A list of integer problem instances to evaluate
            dims: typing.List[int],
                A list of integer problem dimensions to evaluate
            reps: int = 1,
                The number of independent repetitions for each problem, instance
                dimension combination
            problem_class: ProblemClass = ProblemClass.REAL
                The type of problems to test.
            njobs: int = 1
                The number of parallel jobs, -1 assigns all available cpu's,
            logged: bool = True
                Whether or not the experiment uses a logger
            logger_triggers: typing.List[logger.trigger.Trigger]
                A list of trigger instances, used to determine when to trigger
            logger_additional_properties: typing.List[logger.property.AbstractProperty]
                A list of additional properties to be logged. If the properties to be logged are
                members of `algorithm`, you can also provide a list of attribute names to
                `logged_attributes`.
            output_directory: str = "./"
                The root output directory for the logger
            folder_name: str = "ioh_data"
                The name of the output directory for the logger
            algorithm_name: str = None
                A name for the algorithm. This is used in the log files.
            algorithm_info: str = ""
                Optional information, additional information used in log files
            optimization_type: OptimizationType = OptimizationType.MIN
                The type of optimization
            store_positions: bool = False
                Whether to store the x-positions in the data-files
            experiment_attributes: typing.Dict[str, str] = []
                Attributes additionally stored per experiment. These are static
                throughout the experiment and stored in the .info files.
            run_attributes: typing.List[str] = []
                Names of attributes which are updated at every run, i.e. run index.
                These are stored in the .info files.
                Note that each of these attributes is required to be an attribute
                on algorithm.
            logged_attributes: typing.List[str] = []
                Names of attributes which are updated during the run of the algoritm,
                and are logged in the data files at every time point there is a
                logging action.
                Note that each of these attributes is required to be an attribute
                on algorithm.
            merge_output: bool = True
                Whether to merge output from multiple folders with the same
                folder_name. When run in parallel, seperate folder are produced.
                This option attempts to merge the folders back in single results.
                Note that this will also merge data already present in the folder,
                when the same name is used.
            zip_output: bool = True
                Whether to produce a .zip folder of output
            remove_data: bool = False
                Whether to remove all the produced data, except for the .zip file
                (when produced).
        '''
        if not isinstance(problem_class, ProblemClass):
           raise AttributeError(f"problem_class should be of type {ProblemClass}")

        self.algorithm = algorithm
        self.logger_root = os.path.realpath(os.path.join(output_directory, folder_name))

        self.logger_params = dict(
            triggers=logger_triggers,
            additional_properties=logger_additional_properties,
            root=output_directory,
            folder_name=folder_name,
            algorithm_name=algorithm_name or str(algorithm),
            algorithm_info=algorithm_info,
            store_positions=store_positions,
        )
        self.fids = fids
        self.iids = iids
        self.dims = dims
        self.reps = reps
        self.logged = logged
        self.problem_class = problem_class
        self.njobs = njobs if njobs != -1 else multiprocessing.cpu_count()
        self.experiment_attributes = experiment_attributes
        self.logged_attributes = logged_attributes
        self.run_attributes = run_attributes
        self.merge_output = merge_output
        self.zip_output = zip_output
        self.remove_data = remove_data
        self.enforce_bounds = enforce_bounds
        self.old_logger = old_logger

        if os.path.isdir(self.logger_root) and self.merge_output:
            warnings.warn(
                f"There is already a folder named {self.logger_root} "
                f"and merge_output is set to True. The results of this experiment "
                f"will be merged with the contents of this folder",
                UserWarning,
            )

        for attr in itertools.chain(self.run_attributes, self.logged_attributes):
            if not hasattr(self.algorithm, attr):
                raise TypeError(
                    f"Attribute {attr} is not a member of algorithm {self.algorithm}"
                )


    def evaluate(self, ii: int, job: typing.Tuple[int, int, int]):
        '''Evaluate a single function using self.algoritm.

        Note that this functions makes a copy of the algorithm for each new problem
        instantiation.

        Parameters
        ----------
        ii: int
            Iterator id
        job[fid: int, iid: int, dim: int]
            The problem id, the instance id and the problem dimension
        '''
        algorithm = copy.deepcopy(self.algorithm)
        p = get_problem(*job, self.problem_class)
        if self.logged:
            logger_params = copy.deepcopy(self.logger_params)
            logger_params["folder_name"] += f"-tmp-{ii}"
            
            logger_cls = logger.old.Analyzer if self.old_logger else logger.Analyzer
            l = logger_cls(**logger_params) 
            l.set_experiment_attributes(self.experiment_attributes)
            l.add_run_attributes(algorithm, self.run_attributes)
            l.watch(algorithm, self.logged_attributes)
            l.reset()
            p.attach_logger(l)

        if self.enforce_bounds:
            p.enforce_bounds()

        self.apply(algorithm, p)

    def apply(self, algorithm: any, problem: ProblemType):
        '''Apply a given algorithm to a problem'''
        for _ in range(self.reps):
            algorithm(problem)
            problem.reset()

    def add_custom_problem(self, p: callable, name: str = None, **kwargs):
        '''Add a custom problem to the list of functions to be evaluated.

        Parameters
        ---------
        p: callable
            A function which takes an list of numeric values and return as list
            of numeric values

        name: str
            An optional name of the the newly added function.
        '''

        name = name or "CustomProblem"

        if not self.problem_class.is_real():
            problem.wrap_integer_problem(p, name, **kwargs)
        else:
            problem.wrap_real_problem(p, name, **kwargs)

        self.fids.append(get_problem_id(name, self.problem_class))

    def merge_output_to_single_folder(self, prefix: str, target_folder: str):
        '''Merges all ioh data folders into a single folder, having the same folder name prefix

        Parameters
        ----------
        prexix: str
            The prefix on which to select folders
        target_folder: str
            The target folder, i.e. the folder with the final output
        '''


        def merge_info_file(target, source):                        
            ext = ".info" if self.old_logger else ".json"

            if not (target.endswith(ext) and source.endswith(ext)):
                raise RuntimeError("Merging output with incompatible folders")

            has_file = os.path.isfile(target)
            if not has_file:
                return os.rename(source, target)
            with open(source) as info_in:
                if self.old_logger:
                    with open(target, "a+") as info_out:        
                        info_out.write("\n")
                        for line in info_in:
                            info_out.write(line)
                else:
                    with open(target, "r+") as info_out:
                        data_out = json.loads(info_out.read())
                        data_in = json.loads(info_in.read())
                        for scen in data_in['scenarios']:
                            try:
                                scen_out, *_ = [s for s in data_out['scenarios'] if s['dimension'] == scen['dimension']]
                                scen_out['runs'].extend(scen['runs'])
                            except:
                                data_out['scenarios'].append(scen)
                        info_out.seek(0)
                        info_out.write(json.dumps(data_out, indent=4))
                        info_out.truncate()
            os.remove(source)
                    

        def file_to_dir(path):
            root, dirname = os.path.split(os.path.splitext(path)[0])
            return os.path.join(root, dirname.replace("IOHprofiler", "data"))

        root = os.path.dirname(self.logger_root)

        for folder_name in os.listdir(root):
            folder = os.path.join(root, folder_name)
            if not os.path.isdir(folder):
                continue

            if folder_name.startswith(prefix) and folder_name != os.path.basename(
                target_folder
            ):
                folder = os.path.join(root, folder_name)

                for info_file in os.listdir(folder):
                    source = os.path.join(folder, info_file)
                    if not os.path.isfile(source):
                        continue
                    
                    target = os.path.join(target_folder, info_file)
                    merge_info_file(target, source)

                    source_dat_folder = file_to_dir(source)
                    target_dat_folder = file_to_dir(target)

                    os.makedirs(target_dat_folder, exist_ok=True)
                    for dat_file in os.listdir(source_dat_folder):
                        source_dat_file = os.path.join(source_dat_folder, dat_file)
                        target_dat_file = os.path.join(target_dat_folder, dat_file)

                        with open(source_dat_file) as dat_in, open(
                            target_dat_file, "a+"
                        ) as dat_out:
                            for line in dat_in:
                                dat_out.write(line)

                        os.remove(source_dat_file)
                    os.removedirs(source_dat_folder)

    def merge_tmp_folders(self):
        '''Merges the output of the temporary folders of the experiment'''

        root = os.path.basename(self.logger_root)
        target, idx = self.logger_root, 1
        while os.path.exists(target):
            target = root + f"-{idx}"
            idx += 1

        os.rename(self.logger_root + "-tmp-1", target)
        self.merge_output_to_single_folder(root + "-tmp", target)

    def run(self):
        '''Alias for __call__'''

        return self()

    def __call__(self):
        '''Run the experiment'''

        iterator = enumerate(itertools.product(self.fids, self.iids, self.dims), 1)
        if self.njobs != 1:
            print(self.njobs)
            with multiprocessing.Pool(self.njobs) as p:
                p.starmap(self.evaluate, iterator)
        else:
            for job in iterator:
                self.evaluate(*job)

        self.merge_tmp_folders()

        if self.merge_output:
            self.merge_output_to_single_folder(
                os.path.basename(self.logger_root), self.logger_root
            )

        if self.zip_output:
            shutil.make_archive(self.logger_root, "zip", self.logger_root)

        if self.remove_data:
            shutil.rmtree(self.logger_root)

        return self


__all__ = (
    "get_problem",
    "wrap_problem",
    "get_problem_id",
    "Experiment"
)

