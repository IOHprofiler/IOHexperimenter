# EAH


### _class_ ioh.iohcpp.logger.EAH(\*args, \*\*kwargs)
Bases: [`ioh.iohcpp.logger.Logger`](ioh.iohcpp.logger.Logger.md#ioh.iohcpp.logger.Logger)

Overloaded function.


1. __init__(self: ioh.iohcpp.logger.EAH, error_min: float, error_max: float, error_buckets: int, evals_min: int, evals_max: int, evals_buckets: int) -> None


2. __init__(self: ioh.iohcpp.logger.EAH, error_scale: ioh.iohcpp.logger.eah.LinearRealScale, eval_scale: ioh.iohcpp.logger.eah.LinearIntegerScale) -> None


3. __init__(self: ioh.iohcpp.logger.EAH, error_scale: ioh.iohcpp.logger.eah.Log2RealScale, eval_scale: ioh.iohcpp.logger.eah.Log2IntegerScale) -> None


4. __init__(self: ioh.iohcpp.logger.EAH, error_scale: ioh.iohcpp.logger.eah.Log10RealScale, eval_scale: ioh.iohcpp.logger.eah.Log10IntegerScale) -> None

### Attributes Summary

| `data`
 | 

 |
| `error_range`
                                | 

                                                                                     |
| `eval_range`
                                 | 

                                                                                     |
| `size`
                                       | 

                                                                                     |
### Methods Summary

| `at`(self, arg0, arg1, arg2, arg3)

           | 

                                                                                     |
### Attributes Documentation


#### data()

#### error_range()

#### eval_range()

#### size()
### Methods Documentation


#### at(self: ioh.iohcpp.logger.EAH, arg0: int, arg1: int, arg2: int, arg3: int)
