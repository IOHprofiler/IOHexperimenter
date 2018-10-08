""" The function to read the configuration from `.ini` file.
    Only read interface is applied. Please modify the `.ini` by the file,
    or use the write interfaces from ConfigParser by yourself. 
"""

import sys,os
import ConfigParser

class configuration:
	suite_name = ''
	functions_id = ''
	instances_id = ''
	dimensions = ''
	observer_name = ''
	result_folder = ''
	algorithm_name  = ''
	algorithm_info = ''
	parameters_name = ''
	complete_triggers = ''
	base_evaluation_triggers = ' '
	number_target_triggers = 0
	number_interval_triggers = 0
	def set_cfg(self, filename):
		self.configuration_file = filename
	def readcfg(self):
		conf = ConfigParser.ConfigParser()
		conf.read(self.configuration_file)
		self.suite_name = conf.get("suite","suite_name")
		self.functions_id = conf.get("suite","functions_id")
		self.instances_id = conf.get("suite","instances_id")
		self.dimensions = conf.get("suite","dimensions")

		self.observer_name = conf.get("observer","observer_name")
		self.result_folder = conf.get("observer","result_folder")
		self.algorithm_name = conf.get("observer","algorithm_name")
		self.algorithm_info = conf.get("observer","algorithm_info")
		self.parameters_name = conf.get("observer","parameters_name")

		self.complete_triggers = conf.get("triggers","complete_triggers")
		self.base_evaluation_triggers = conf.get("triggers","base_evaluation_triggers")
		self.number_target_triggers = conf.get("triggers","number_target_triggers")
		self.number_interval_triggers = conf.get("triggers","number_interval_triggers")



