# wrap_problem


### ioh.wrap_problem(function: Callable[[List[Union[int, float]]], float], name: str, problem_type: str, dimension: int = 5, instance: int = 1, optimization_type: ioh.iohcpp.OptimizationType = <OptimizationType.Minimization: 0>, lb: Optional[Union[int, float]] = None, ub: Optional[Union[int, float]] = None, transform_variables: Optional[Callable[[List[Union[int, float]], int], List[Union[int, float]]]] = None, transform_objectives: Optional[Callable[[float, int], float]] = None, calculate_objective: Optional[Callable[[int, int], Union[ioh.iohcpp.IntegerSolution, ioh.iohcpp.RealSolution]]] = None)
Function to wrap a callable as an ioh function


* **Parameters**

    
    * **function** (*fn**(**x: list**) **-> float*) – The callable to wrap


    * **name** (*str*) – The name of the function. This can be used to create new instances of this function.
    Note, when not using unique names, this will override the previously wrapped functions.


    * **problem_type** (*str*) – The type of the problem, accepted types are: ‘Real’ and ‘Integer’


    * **dimension** (*int*) – The dimension (number of variables) of the problem


    * **instance** (*int*) – The instance ID of the problem


    * **optimization_type** ([*OptimizationType*](ioh.OptimizationType.md#ioh.OptimizationType)) – The type of optimization to do, maximization or minimization


    * **lb** (*[**int**, **float**]*) – The lower bound of the constraint, should be the same type as problem_type. When left to None, this will set
    to the lowest possible value for that type.


    * **ub** (*[**int**, **float**]*) – The upper bound of the constraint, should be the same type as problem_type. When left to None, this will set
    to the highest possible value for that type.


    * **transform_variables** (*fn**(**x: list**) **-> list*) – A function to transform the elements of x, prior to calling the function.


    * **transform_objectives** (*fn**(**y: float**) **-> float*) – A function to tranform the float value of y, after callling the function.


    * **calculate_objective** (*fn**(**instance**, **dimension**) **-> Solution** | **(**x**, **y**)*) – A function to calculate the global optimum of the function. This function gets a dimension and instance id,
    and should return either a Solution objective(IntegerSolution or RealSolution) or a tuple giving the
    x and y values for the global optimum. Where x is the search space representation and y the target value.
