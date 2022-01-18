# get_problem


### ioh.get_problem(fid: Union[int, str], instance: int = 1, dimension: int = 5, problem_type: str = 'Real')
Instantiate a problem based on its function ID, dimension, instance and suite


* **Parameters**

    
    * **fid** (*int** or **str*) – The function ID of the problem in the suite, or the name of the function as string


    * **dimension** (*int*) – The dimension (number of variables) of the problem


    * **instance** (*int*) – The instance ID of the problem


    * **problem_type** (*str*) – Which suite the problem is from. Either ‘BBOB’ or ‘PBO’ or ‘Real’ or ‘Integer’
    Only used if fid is an int.
