# Analyzer


### _class_ ioh.iohcpp.logger.Analyzer(self: ioh.iohcpp.logger.Analyzer, triggers: List[ioh.iohcpp.logger.trigger.Trigger] = [<ioh.iohcpp.logger.trigger.OnImprovement object at 0x7fb967d97830>], additional_properties: List[ioh.iohcpp.logger.property.AbstractProperty] = [], root: ioh.iohcpp.logger.Path = <ioh.iohcpp.logger.Path object at 0x7fb967d977f0>, folder_name: str = 'ioh_data', algorithm_name: str = 'algorithm_name', algorithm_info: str = 'algorithm_info', store_positions: bool = False)
Bases: [`ioh.iohcpp.logger.AbstractWatcher`](ioh.iohcpp.logger.AbstractWatcher.md#ioh.iohcpp.logger.AbstractWatcher)

### Attributes Summary

| `output_directory`
 | 

 |
### Methods Summary

| `add_experiment_attribute`(self, arg0, arg1)

 | 

                                                                                     |
| `add_run_attributes`(\*args, \*\*kwargs)

        | Overloaded function.

                                                                 |
| `set_experiment_attributes`(self, arg0)

      | 

                                                                                     |
| `set_run_attribute`(self, arg0, arg1)

        | 

                                                                                     |
| `set_run_attributes`(self, arg0)

             | 

                                                                                     |
| `watch`(\*args, \*\*kwargs)

                     | Overloaded function.

                                                                 |
### Attributes Documentation


#### output_directory()
### Methods Documentation


#### add_experiment_attribute(self: ioh.iohcpp.logger.Analyzer, arg0: str, arg1: str)

#### add_run_attributes(\*args, \*\*kwargs)
Overloaded function.


1. add_run_attributes(self: ioh.iohcpp.logger.Analyzer, arg0: str, arg1: float) -> None


2. add_run_attributes(self: ioh.iohcpp.logger.Analyzer, arg0: object, arg1: str) -> None


3. add_run_attributes(self: ioh.iohcpp.logger.Analyzer, arg0: object, arg1: List[str]) -> None


#### set_experiment_attributes(self: ioh.iohcpp.logger.Analyzer, arg0: Dict[str, str])

#### set_run_attribute(self: ioh.iohcpp.logger.Analyzer, arg0: str, arg1: float)

#### set_run_attributes(self: ioh.iohcpp.logger.Analyzer, arg0: Dict[str, float])

#### watch(\*args, \*\*kwargs)
Overloaded function.


1. watch(self: ioh.iohcpp.logger.Analyzer, arg0: ioh.iohcpp.logger.property.AbstractProperty) -> None


2. watch(self: ioh.iohcpp.logger.Analyzer, arg0: object, arg1: str) -> None


3. watch(self: ioh.iohcpp.logger.Analyzer, arg0: object, arg1: List[str]) -> None
