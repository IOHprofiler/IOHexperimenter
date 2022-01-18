# IntegerBase


### _class_ ioh.iohcpp.suite.IntegerBase()
Bases: `pybind11_builtins.pybind11_object`

IntegerBase suite

### Attributes Summary

| `dimensions`
 | The list of all problems ids contained in the current suite.

 |
| `instances`
                                    | The list of all instance ids contained in the current suite.

                                   |
| `name`
                                         | The name of the suite.

                                                                         |
| `problem_ids`
                                  | The list of all problems ids contained in the current suite.

                                   |
### Methods Summary

| `attach_logger`(self, arg0)

                    | Attach a logger to the problem to track the performance and internal variables.

                |
| `detach_logger`(self)

                          | Remove the specified logger from the problem

                                                   |
| `reset`(self)

                                  | Reset the state variables of the current problem.

                                              |
### Attributes Documentation


#### dimensions()
The list of all problems ids contained in the current suite.


#### instances()
The list of all instance ids contained in the current suite.


#### name()
The name of the suite.


#### problem_ids()
The list of all problems ids contained in the current suite.

### Methods Documentation


#### attach_logger(self: ioh.iohcpp.suite.IntegerBase, arg0: [ioh.iohcpp.logger.Logger](ioh.iohcpp.logger.Logger.md#ioh.iohcpp.logger.Logger))
Attach a logger to the problem to track the performance and internal variables.


#### detach_logger(self: ioh.iohcpp.suite.IntegerBase)
Remove the specified logger from the problem


#### reset(self: ioh.iohcpp.suite.IntegerBase)
Reset the state variables of the current problem.
