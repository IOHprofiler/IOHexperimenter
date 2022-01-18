# ioh

## ioh Package

Python interface of IOH package.

### Functions

| [`get_problem`](api/ioh.get_problem.md#ioh.get_problem)(fid[, instance, dimension, ...])

 | Instantiate a problem based on its function ID, dimension, instance and suite

 |
| [`get_problem_id`](api/ioh.get_problem_id.md#ioh.get_problem_id)(problem_name, problem_type)

   | 

                                                                                               |
| [`wrap_problem`](api/ioh.wrap_problem.md#ioh.wrap_problem)(function, name, problem_type[, ...])

 | Function to wrap a callable as an ioh function

                                                 |
### Classes

| [`Experiment`](api/ioh.Experiment.md#ioh.Experiment)(algorithm, fids, iids, dims, ...)

      | 
* **param algorithm**

    A instance of a given algorithm. Must implement a __call__ method



             |
| [`IntegerConstraint`](api/ioh.IntegerConstraint.md#ioh.IntegerConstraint)
                                 | 

                                                                                               |
| [`IntegerSolution`](api/ioh.IntegerSolution.md#ioh.IntegerSolution)
                                   | Initialize a Solution object using its coordinates and fitness.

                                |
| [`IntegerState`](api/ioh.IntegerState.md#ioh.IntegerState)(self, arg0)

                          | 

                                                                                               |
| [`LogInfo`](api/ioh.LogInfo.md#ioh.LogInfo)(self, arg0, arg1, arg2, arg3, arg4, arg5)

 | 

                                                                                               |
| [`MetaData`](api/ioh.MetaData.md#ioh.MetaData)(self, arg0, arg1, arg2, arg3, arg4)

      | 

                                                                                               |
| [`OptimizationType`](api/ioh.OptimizationType.md#ioh.OptimizationType)(self, value)

                     | Enum used for defining wheter the problem is Mimization or Maximization

                        |
| [`RealConstraint`](api/ioh.RealConstraint.md#ioh.RealConstraint)
                                    | 

                                                                                               |
| [`RealSolution`](api/ioh.RealSolution.md#ioh.RealSolution)
                                      | Initialize a Solution object using its coordinates and fitness.

                                |
| [`RealState`](api/ioh.RealState.md#ioh.RealState)(self, arg0)

                             | 

                                                                                               |
### Class Inheritance Diagram
