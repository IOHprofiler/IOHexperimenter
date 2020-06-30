import IOHexperimenter as IOH
import numpy as np
from pathlib import Path
from collections.abc import Iterable

class IOH_logger:
    def __init__(self, location, foldername = "run", name = "", info = ""):
        '''Creates a logger to be used within an IOH_problem object

        Parameters
        ----------
        location:
            The directory in which to store the results
        foldername:
            The name of the folder to create to store the results
        name:
            The name of the algorithm intened to run on this function
        info:
            Additional information on the algorithm
        
        Returns
        ------
        A logger from the IOHexperimenter package
        '''
        Path(location).mkdir(parents=True, exist_ok=True)
        self.logger = IOH.IOHprofiler_csv_logger(location, foldername, name, info)
        self.logger.activate_logger();
        self.params = []
        self.dynamic_attrs = []
        self.alg = None
        self.set_tracking_options()
    
    def set_tracking_options(self, dat = True, cdat = False, idat = 0, tdat_base = [0], tdat_exp = 0):
        '''Set which datafiles should be stored

        Parameters
        ----------
        dat:
            Whether or not to store .dat-files (one line per improvement of objective)
        cdat:
            Whether or not to store .cdat-files (one line for each evaluation of objective)
        idat:
            Integer, if non-zero, an .idat file will be made which records ever i-th evaluation of the objective
        tdat_base:
            Base values for the tdat-file
        tdat_exp:
            Exponent-values for the tdat-file
        '''
        self.logger.set_complete_flag(cdat); #Set to True to enable cdat
        self.logger.set_interval(idat); #Set to non-zero to enable idat
        self.logger.set_time_points(tdat_base, tdat_exp) #tdat
        self.logger.set_update_flag(dat) #dat
        
    def track_problem(self, problem_id, nr_variables, instance_id, problem_name, optimization_type, suite = "No Suite"):
        '''Set the problem-specific variables to store in the info-file

        Parameters
        ----------
        problem_id:
            Numerical identifier for the problem
        nr_variables:
            How many variables (dimension) the problem has
        instance_id:
            The numerical instance id
        problem_name:
            The name of the problem as a string
        optimization_type:
            Boolean. True for maximization, False for mimimization
        suite:
            Optional. The suite the problem comes from
        '''
        self.logger.track_problem(problem_id, nr_variables,
                              instance_id, problem_name,
                              optimization_type)
        self.logger.track_suite(suite)
        
    def track_parameters(self, algorithm, parameters):
        '''Initialize tracking of parameters during the run of the algorithm

        Parameters
        ----------
        algorithm:
            The algorithm object. Can also be a parameter subobject. The only requirement is that all
            'parameters'-variables are accessible by using algorithm.parameter (recommended to use @property)
        parameters:
            A list of parameters to track
        '''
        if isinstance(parameters, str):
            self.params = [parameters]
        elif isinstance(parameters, Iterable):
            self.params = parameters
        else:
            raise typeError("parameters needs to be a string or iterable containing strings")
        self.logger.set_parameters_name(self.params)
        self.alg = algorithm
        
    def set_static_attributes(self, attrs):
        '''Set static attributes to store in the info-files. Make sure to only 
        call after setting the tracked algorithm using track_parameters.

        Parameters
        ----------
        attrs:
            A list of parameters to track (should be accessible in the tracked algorithm)
        '''
        if isinstance(attrs, str):
            exec(f"self.logger.add_attribute('{attrs}', self.alg.{attrs})")
        elif isinstance(attrs, Iterable):
            for a in attrs:
                exec(f"self.logger.add_attribute('{a}', self.alg.{a})")
        else:
            raise typeError("attrs needs to be a string or iterable containing strings")
            
    def set_dynamic_attributes(self, attrs):
        '''Initialize tracking of parameters during the run of the algorithm. Make sure to only 
        call after setting the tracked algorithm using track_parameters.

        Parameters
        ----------
        attrs:
            A list of parameters to track (should be accessible in the tracked algorithm)
        '''
        if isinstance(attrs, str):
            self.dynamic_attrs = [attrs]
        elif isinstance(attrs, Iterable):
            self.dynamic_attrs = attrs
        else:
            raise typeError("attrs needs to be a string or iterable containing strings")
        self.logger.set_dynamic_attributes_name(self.dynamic_attrs)

    def process_evaluation(self, info):
        '''Internal function for processing evalutation

        Parameters
        ----------
        info:
            The specific information from the function which needs to be processed
        '''
        self.logger.write_line(info)
        
    def process_parameters(self):
        '''Internal function for processing parameter changes

        '''
        for param in self.params:
            exec(f"self.logger.set_parameters(['{param}'], [self.alg.{param}])")
        for param in self.dynamic_attrs:
            exec(f"self.logger.set_dynamic_attributes(['{param}'], [self.alg.{param}])")
    
    
    def clear_logger(self):
        '''Function to clear the logger once it is no longer needed. Finishes all the writing to file which is still in underlying buffers.

        '''
        self.logger.clear_logger()