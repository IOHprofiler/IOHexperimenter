# Real


### _class_ ioh.iohcpp.problem.Real()
Bases: `pybind11_builtins.pybind11_object`

### Attributes Summary

| `constraint`
 | The constraints (bounds) of the problem.

 |
| `log_info`
                                     | Check what data is being sent to the logger.

                                                   |
| `meta_data`
                                    | these variables are static during the lifetime of the problem.

                                 |
| `objective`
                                    | The optimal point and value for the current instanciation of the problem.

                      |
| `problems`
                                     | 

                                                                                               |
| `state`
                                        | The current state of the problem all variables which change during the optimization procedure.

 |
### Methods Summary

| `__call__`
                                     | Evaluate the problem.

                                                                          |
| `attach_logger`
                                | Attach a logger to the problem to allow performance tracking.

                                  |
| `create`
                                       | Create a problem instance Create a problem instance

                                            |
| `detach_logger`
                                | Remove the specified logger from the problem.

                                                  |
| `reset`
                                        | Reset all state-variables of the problem.

                                                      |
### Attributes Documentation


#### constraint()
The constraints (bounds) of the problem.


#### log_info()
Check what data is being sent to the logger.


#### meta_data()
these variables are static during the lifetime of the problem.


* **Type**

    The meta-data of the problem



#### objective()
The optimal point and value for the current instanciation of the problem.


#### problems()

#### state()
The current state of the problem all variables which change during the optimization procedure.

### Methods Documentation


#### \__call__()
Evaluate the problem.


* **Parameters**

    **x** (*a 1-dimensional array / list of size equal to the dimension of this problem*) – 



#### attach_logger()
Attach a logger to the problem to allow performance tracking.


* **Parameters**

    **logger** (*A logger-object from the IOHexperimenter 'logger' module.*) – 



#### _static_ create()
Create a problem instance
Create a problem instance


#### detach_logger()
Remove the specified logger from the problem.


* **Parameters**

    **logger** (*A logger-object from the IOHexperimenter 'logger' module.*) – 



#### reset()
Reset all state-variables of the problem.
