'''Python specific functions for IOH package

TODO: best far precision
TODO: tracking of out-of bounds
TODO: Check quadratic function, min dimension bbob=2
TODO: Rename Integer -> Discrete
'''

import os
import math
import warnings
import itertools
import multiprocessing
import typing
import shutil
import copy

try:
    from .iohcpp import (
        problem, 
        suite, 
        logger,
        OptimizationType,
        RealSolution,
        IntegerSolution, 
        IntegerConstraint,
        RealConstraint,
        RealState,
        IntegerState,
        MetaData,
        LogInfo
    )
except ModuleNotFoundError:
    raise ModuleNotFoundError("No module named ioh")


ProblemType = typing.Union[problem.Real, problem.Integer]

def get_problem(fid: int, iid: int, dim: int, problem_type: str = "Real") -> ProblemType:
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
    if problem_type in ("BBOB", "Real",):
        return getattr(problem, problem_type).create(fid, iid, dim)
    elif problem_type in ("PBO", "Integer",):
        if fid in [21, 23, "IsingTriangular", "NQueens"]:
            if not math.sqrt(dim).is_integer():
                raise ValueError("For this function, the dimension needs to be a perfect square!")
        return getattr(problem, problem_type).create(fid, iid, dim)

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
        logger_triggers: typing.List[logger.trigger.Trigger] = [
            logger.trigger.ON_IMPROVEMENT
        ],
        logger_additional_properties: typing.List[logger.property.AbstractProperty] = [],
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
        remove_data: bool = False
    ): 
        """
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
            problem_type: str = "BBOB"
                The type of problems to test. Available are "PBO", "BBOB",  "Integer"
                and "Real". Note that "Integer" and "Real" are supersets of "PBO"
                and "BBOB" resp.
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
            optimization_type: OptimizationType = OptimizationType.Minimization
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
        """
        
        self.algorithm = algorithm
        self.logger_root = os.path.join(output_directory, folder_name)

        self.logger_params = dict(
            triggers = logger_triggers,
            additional_properties = logger_additional_properties,
            root = output_directory,
            folder_name = folder_name,
            algorithm_name = algorithm_name or str(algorithm),
            algorithm_info = algorithm_info,
            store_positions = store_positions,
        )
        self.fids = fids
        self.iids = iids 
        self.dims = dims 
        self.reps = reps
        self.logged = logged
        self.problem_type = problem_type
        self.njobs = njobs if njobs != -1 else multiprocessing.cpu_count()
        self.experiment_attributes = experiment_attributes
        self.logged_attributes = logged_attributes
        self.run_attributes = run_attributes
        self.merge_output = merge_output
        self.zip_output = zip_output
        self.remove_data = remove_data
        
        if os.path.isdir(self.logger_root) and self.merge_output:
            warnings.warn(
                f"There is already a folder named {self.logger_root} "
                f"and merge_output is set to True. The results of this experiment "
                f"will be merged with the contents of this folder", UserWarning)
        

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
            l = logger.Analyzer(**self.logger_params)
            l.set_experiment_attributes(self.experiment_attributes)
            l.add_run_attributes(self.algorithm, self.run_attributes)
            l.watch(self.algorithm, self.logged_attributes)

            p.attach_logger(l)
            self.apply(algorithm, p)
        else:
            self.apply(algorithm, p)
            

    def apply(self, algorithm: any, problem: ProblemType):
        '''Apply a given algorithm to a problem'''
        
        for i in range(self.reps):
            algorithm(problem) 
            problem.reset()       
 
    def add_custom_problem(self, p: ProblemType, name: str = None):
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

    def merge_output_to_single_folder(self):
        """Merges all ioh data folders into a single folder, having the same 
        folder name prefix: self.logger_root
        
        """

        root = os.path.dirname(self.logger_root)
        for folder_name in os.listdir(root):
            folder = os.path.join(root, folder_name)
            if not os.path.isdir(folder): continue

            if folder_name.startswith(os.path.basename(self.logger_root))\
                and folder_name != os.path.basename(self.logger_root):
                folder = os.path.join(root, folder_name)
                for info_file in os.listdir(folder):
                    source = os.path.join(folder, info_file)
                    if not os.path.isfile(source): continue

                    if not info_file.endswith(".info"):
                        raise RuntimeError("Merging output with incompatible folders")

                    target = os.path.join(self.logger_root, info_file)
                    target_exists = os.path.isfile(target)

                    source_dat_folder = os.path.splitext(source)[0].replace("IOHprofiler", "data")
                    target_dat_folder = os.path.splitext(target)[0].replace("IOHprofiler", "data")

                    with open(source) as info_in, open(target, "a+") as info_out:
                        if target_exists:
                            info_out.write("\n")
                        for line in info_in:
                            info_out.write(line)
                    os.remove(source)

                    os.makedirs(target_dat_folder, exist_ok=True)
                    for dat_file in os.listdir(source_dat_folder):
                        source_dat_file = os.path.join(source_dat_folder, dat_file)
                        target_dat_file = os.path.join(target_dat_folder, dat_file)

                        with open(source_dat_file) as dat_in, open(target_dat_file, "a+") as dat_out:
                            for line in dat_in:
                                dat_out.write(line)

                        os.remove(source_dat_file)
                    os.removedirs(source_dat_folder)

    def run(self):
        """Alias for __call__"""

        return self()

    def __call__(self):
        '''Run the experiment'''

        iterator = itertools.product(self.fids, self.iids, self.dims)
        if self.njobs != 1:
            with multiprocessing.Pool(self.njobs) as p:
                p.starmap(self.evaluate, iterator)
        else:
            for fid, iid, dim in iterator:
                self.evaluate(fid, iid, dim)

        if self.merge_output:
            self.merge_output_to_single_folder()

        if self.zip_output:
            shutil.make_archive(self.logger_root, 'zip', self.logger_root)
        
        if self.remove_data:
            shutil.rmtree(self.logger_root)
    
        return self

