# Experiment


### _class_ ioh.Experiment(algorithm: Any, fids: List[int], iids: List[int], dims: List[int], reps: int = 1, problem_type: str = 'Real', njobs: int = 1, logged: bool = True, logger_triggers: List[ioh.iohcpp.logger.trigger.Trigger] = [<ioh.iohcpp.logger.trigger.OnImprovement object>], logger_additional_properties: List[ioh.iohcpp.logger.property.AbstractProperty] = [], output_directory: str = './', folder_name: str = 'ioh_data', algorithm_name: Optional[str] = None, algorithm_info: str = 'algorithm_info', store_positions: bool = False, experiment_attributes: Dict[str, str] = {}, run_attributes: List[str] = [], logged_attributes: List[str] = [], merge_output: bool = True, zip_output: bool = True, remove_data: bool = False)
Bases: `object`


* **Parameters**

    
    * **algorithm** (*typing.Any*) – A instance of a given algorithm. Must implement a __call__ method
    taking a ioh.problem as argument.


    * **fids** (*typing.List**[**int**]*) – A list of integer problem ids to evaluate


    * **iids** (*typing.List**[**int**]**,*) – A list of integer problem instances to evaluate


    * **dims** (*typing.List**[**int**]**,*) – A list of integer problem dimensions to evaluate


    * **reps** (*int = 1**,*) – The number of independent repetitions for each problem, instance
    dimension combination


    * **problem_type** (*str = "BBOB"*) – The type of problems to test. Available are “PBO”, “BBOB”,  “Integer”
    and “Real”. Note that “Integer” and “Real” are supersets of “PBO”
    and “BBOB” resp.


    * **njobs** (*int = 1*) – The number of parallel jobs, -1 assigns all available cpu’s,


    * **logged** (*bool = True*) – Whether or not the experiment uses a logger


    * **logger_triggers** (*typing.List**[**logger.trigger.Trigger**]*) – A list of trigger instances, used to determine when to trigger


    * **logger_additional_properties** (*typing.List**[**logger.property.AbstractProperty**]*) – A list of additional properties to be logged. If the properties to be logged are
    members of algorithm, you can also provide a list of attribute names to
    logged_attributes.


    * **output_directory** (*str = "./"*) – The root output directory for the logger


    * **folder_name** (*str = "ioh_data"*) – The name of the output directory for the logger


    * **algorithm_name** (*str = None*) – A name for the algorithm. This is used in the log files.


    * **algorithm_info** (*str = ""*) – Optional information, additional information used in log files


    * **optimization_type** (*OptimizationType = OptimizationType.Minimization*) – The type of optimization


    * **store_positions** (*bool = False*) – Whether to store the x-positions in the data-files


    * **experiment_attributes** (*typing.Dict**[**str**, **str**] **=** [**]*) – Attributes additionally stored per experiment. These are static
    throughout the experiment and stored in the .info files.


    * **run_attributes** (*typing.List**[**str**] **=** [**]*) – Names of attributes which are updated at every run, i.e. run index.
    These are stored in the .info files.
    Note that each of these attributes is required to be an attribute
    on algorithm.


    * **logged_attributes** (*typing.List**[**str**] **=** [**]*) – Names of attributes which are updated during the run of the algoritm,
    and are logged in the data files at every time point there is a
    logging action.
    Note that each of these attributes is required to be an attribute
    on algorithm.


    * **merge_output** (*bool = True*) – Whether to merge output from multiple folders with the same
    folder_name. When run in parallel, seperate folder are produced.
    This option attempts to merge the folders back in single results.
    Note that this will also merge data already present in the folder,
    when the same name is used.


    * **zip_output** (*bool = True*) – Whether to produce a .zip folder of output


    * **remove_data** (*bool = False*) – Whether to remove all the produced data, except for the .zip file
    (when produced).


### Methods Summary

| `__call__`()

 | Run the experiment

 |
| `add_custom_problem`(p[, name])

 | Add a custom problem to the list of functions to be evaluated.

 |
| `apply`(algorithm, problem)

     | Apply a given algorithm to a problem

                           |
| `evaluate`(ii, job)

             | Evaluate a single function using self.algoritm.

                |
| `merge_output_to_single_folder`(prefix, ...)

 | Merges all ioh data folders into a single folder, having the same folder name prefix

 |
| `merge_tmp_folders`()

                        | Merges the output of the temporary folders of the experiment

                         |
| `run`()

                                      | Alias for __call__

                                                                   |
### Methods Documentation


#### \__call__()
Run the experiment


#### add_custom_problem(p: Union[[ioh.iohcpp.problem.Real](ioh.iohcpp.problem.Real.md#ioh.iohcpp.problem.Real), [ioh.iohcpp.problem.Integer](ioh.iohcpp.problem.Integer.md#ioh.iohcpp.problem.Integer)], name: Optional[str] = None, \*\*kwargs)
Add a custom problem to the list of functions to be evaluated.


* **Parameters**

    
    * **p** (*callable*) – A function which takes an list of numeric values and return as list
    of numeric values


    * **name** (*str*) – An optional name of the the newly added function.



#### apply(algorithm: any, problem: Union[[ioh.iohcpp.problem.Real](ioh.iohcpp.problem.Real.md#ioh.iohcpp.problem.Real), [ioh.iohcpp.problem.Integer](ioh.iohcpp.problem.Integer.md#ioh.iohcpp.problem.Integer)])
Apply a given algorithm to a problem


#### evaluate(ii: int, job: Tuple[int, int, int])
Evaluate a single function using self.algoritm.

Note that this functions makes a copy of the algorithm for each new problem
instantiation.


* **Parameters**

    
    * **ii** (*int*) – Iterator id


    * **job****[****fid** (*int**, **iid: int**, **dim: int**]*) – The problem id, the instance id and the problem dimension



#### merge_output_to_single_folder(prefix: str, target_folder: str)
Merges all ioh data folders into a single folder, having the same folder name prefix


* **Parameters**

    
    * **prexix** (*str*) – The prefix on which to select folders


    * **target_folder** (*str*) – The target folder, i.e. the folder with the final output



#### merge_tmp_folders()
Merges the output of the temporary folders of the experiment


#### run()
Alias for __call__
