# FlatFile


### _class_ ioh.iohcpp.logger.FlatFile(self: ioh.iohcpp.logger.FlatFile, triggers: List[ioh.iohcpp.logger.trigger.Trigger], properties: List[ioh.iohcpp.logger.property.AbstractProperty], filename: str = 'IOH.dat', output_directory: [ioh.iohcpp.logger.Path](ioh.iohcpp.logger.Path.md#ioh.iohcpp.logger.Path) = './', separator: str = '\\t', comment: str = '#', no_value: str = 'None', end_of_line: str = '\\n', repeat_header: bool = False, store_positions: bool = False, common_header_titles: List[str] = ['suite_name', 'problem_name', 'problem_id', 'problem_instance', 'optimization_type', 'dimension', 'run'])
Bases: [`ioh.iohcpp.logger.AbstractWatcher`](ioh.iohcpp.logger.AbstractWatcher.md#ioh.iohcpp.logger.AbstractWatcher)

### Attributes Summary

| `filename`
 | 

 |
| `output_directory`
                           | 

                                                                                     |
### Methods Summary

| `watch`(\*args, \*\*kwargs)

                     | Overloaded function.

                                                                 |
### Attributes Documentation


#### filename()

#### output_directory()
### Methods Documentation


#### watch(\*args, \*\*kwargs)
Overloaded function.


1. watch(self: ioh.iohcpp.logger.FlatFile, arg0: ioh.iohcpp.logger.property.AbstractProperty) -> None


2. watch(self: ioh.iohcpp.logger.FlatFile, arg0: object, arg1: str) -> None


3. watch(self: ioh.iohcpp.logger.FlatFile, arg0: object, arg1: List[str]) -> None
