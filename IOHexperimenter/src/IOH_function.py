from .IOH_Utils import runParallelFunction
from .IOH_logger import IOH_logger
from .. import IOHprofiler as IOH

import numpy as np
from pathlib import Path
import pkg_resources
from packaging import version
from functools import partial
from itertools import product

from collections.abc import Iterable
import operator

bbob_fid_dict = {1:IOH.Sphere, 2:IOH.Ellipsoid, 3:IOH.Rastrigin, 4:IOH.Bueche_Rastrigin, 5:IOH.Linear_Slope, 
                6:IOH.Attractive_Sector, 7:IOH.Step_Ellipsoid, 8:IOH.Rosenbrock, 9:IOH.Rosenbrock_Rotated, 10:IOH.Ellipsoid_Rotated, 
                11:IOH.Discus, 12:IOH.Bent_Cigar, 13:IOH.Sharp_Ridge, 14:IOH.Different_Powers, 15:IOH.Rastrigin_Rotated, 
                16:IOH.Weierstrass, 17:IOH.Schaffers10, 18:IOH.Schaffers1000, 19:IOH.Griewank_RosenBrock, 20:IOH.Schwefel, 
                21:IOH.Gallagher101, 22:IOH.Gallagher21, 23:IOH.Katsuura, 24:IOH.Lunacek_Bi_Rastrigin}

pbo_fid_dict = {1:IOH.OneMax, 2:IOH.LeadingOnes, 3:IOH.Linear, 4:IOH.OneMax_Dummy1, 5:IOH.OneMax_Dummy2, 
                6:IOH.OneMax_Neutrality, 7:IOH.OneMax_Epistasis, 8:IOH.OneMax_Ruggedness1, 9:IOH.OneMax_Ruggedness2, 10:IOH.OneMax_Ruggedness3, 
                11:IOH.LeadingOnes_Dummy1, 12:IOH.LeadingOnes_Dummy2, 13:IOH.LeadingOnes_Neutrality, 14:IOH.LeadingOnes_Epistasis, 
                15:IOH.LeadingOnes_Ruggedness1, 16:IOH.LeadingOnes_Ruggedness2, 17:IOH.LeadingOnes_Ruggedness3, 
                18:IOH.LABS, 19:IOH.MIS, 20:IOH.Ising_Ring, 21:IOH.Ising_Torus, 22:IOH.Ising_Triangular, 23:IOH.NQueens}

class IOH_function():
    '''A wrapper around the functions from the IOHexperimenter
    '''
    def __init__(self, fid, dim, iid, target_precision = 0, suite = "BBOB"):
        '''Instansiate a problem based on its function ID, dimension, instance and suite

        Parameters
        ----------
        fid: 
            The function ID of the problem in the suite, or the name of the function as string
        dim:
            The dimension (number of variables) of the problem
        iid:
            The instance ID of the problem
        target_precision:
            Optional, how close to the optimum the problem is considered 'solved'
        suite:
            Which suite the problem is from. Either 'BBOB' or 'PBO'. Only used if fid is an integer

        '''
        if type(fid) == type(1):
            if suite == "BBOB":
                self.f = bbob_fid_dict[fid](iid, dim)
            elif suite == "PBO":
                if fid in [21, 23]:
                    if not sqrt(n).is_integer():
                        raise Exception("For this function, the dimension needs to be a perfect square!")
                self.f = pbo_fid_dict[fid](iid, dim)
            else:
                raise Exception("This suite is not yet supported")
        else:
            if fid in ["Ising_2D", "NQueens"]:
                if not sqrt(n).is_integer():
                    raise Exception("For this function, the dimension needs to be a perfect square!")
            exec(f"self.f = IOH.{fid}({iid}, {dim})")
        self.f.reset_problem()
        self.precision = target_precision
        self.logger = None        
        self.track_oob = False
        self.suite = suite
        self.maximization = (self.suite == "PBO")
        self.y_comparison = operator.gt if self.maximization else operator.lt
        self.xopt = None
        self.yopt = (self.maximization - 0.5) * -np.inf
    
    def __call__(self, x):
        '''Evaluates the function in point x and deals with logging in needed

        Parameters
        ----------
        x: 
            The point to evaluate

        Returns
        ------
        The value f(x)
        '''
        y = self.f.evaluate(x)
        if self.y_comparison(y, self.yopt):
            self.yopt = y
            self.xopt = x
        if self.track_oob:
            self.oob += any(x < self.lowerbound) or any(x > self.upperbound)
        if self.logger is not None:
            self.logger.process_parameters()
#             for param_obj in self.tracked_params:
#                 print(param_obj)
#                 exec(f"print({param_obj}.__name__)")
#                 exec(f"print({param_obj})")
#                 self.logger.update_parameter(param_obj.__name__, param_obj)
            if self.f.IOHprofiler_get_problem_type() == "bbob":
                self.logger.process_evaluation(self.f.loggerCOCOInfo())
            else:
                self.logger.process_evaluation(self.f.loggerInfo())
        return y
    
    @property
    def final_target_hit(self):
        '''Returns whether the target has been reached

        Returns
        ------
        A boolean indicating whether the optimal value has been reached within the
        pre-defined precision (default 0)
        '''
        if self.f.IOHprofiler_get_problem_type() == "bbob":
            return self.f.IOHprofiler_get_best_so_far_raw_objectives()[0] <= self.precision
        else:
            return self.f.IOHprofiler_get_best_so_far_raw_objectives()[0] >= self.f.IOHprofiler_get_optimal()[0] - self.precision
    
    @property
    def evaluations(self):
        '''Returns number of evaluations performed so far

        Returns
        ------
        Number of evaluations performed so far
        '''
        return self.f.IOHprofiler_get_evaluations()
    
    @property
    def best_so_far_precision(self):
        '''Gets the best-so-far raw function value. Do NOT use directly in 
        the optimization algorithm, since that would break the black-box assumption!
        instead, use best_so_far_fvalue.

        Returns
        ------
        The raw (precision in case of bbob, untransformed objective in case of PBO) objective value
        '''
        return self.f.IOHprofiler_get_best_so_far_raw_objectives()[0]
    
    @property
    def best_so_far_variables(self):
        '''Gets the best variables evaluated so far

        Returns
        ------
        The best-so-far variables
        '''
        return self.xopt

    @property
    def best_so_far_fvalue(self):
        '''Gets the best-so-far function value as seen by the algorithm.

        Returns
        ------
        The best-so-far objective value
        '''
        return self.yopt
    
    @property
    def number_of_variables(self):
        '''Returns the number of variables (dimension) of the problem

        Returns
        ------
        The number of variables (dimension) of the problem
        '''
        return self.f.IOHprofiler_get_number_of_variables()
    
    @property
    def upperbound(self):
        '''Returns the upper bound of the search space
        
        Returns
        ------
        A vector containing the upper bound per coordinate
        '''
        return np.array(self.f.IOHprofiler_get_upperbound())
    
    @property
    def lowerbound(self):
        '''Returns the lower bound of the search space
        
        Returns
        ------
        A vector containing the lower bound per coordinate
        '''
        return np.array(self.f.IOHprofiler_get_lowerbound())
    
    @property
    def number_out_of_bounds(self):
        '''Returns the number of times an out-of-bounds solution was evaluated
        This is only tracked is the parameter 'track_oob' is set to True on intiailization of this object

        Returns
        ------
        The number of the number of out-of-bounds solutions evaluated
        '''
        if self.track_oob:
            return self.oob
        else:
            return np.nan
    
    def add_logger(self, logger):
        '''Adds an observer to the problem

        Parameters
        ----------
        logger: 
            A logger object (of class IOH_logger) with which to track this problem. 
        '''
        if not isinstance(logger, IOH_logger):
            raise TypeError
        logger.track_problem(self.f.IOHprofiler_get_problem_id(), self.f.IOHprofiler_get_number_of_variables(),
                              self.f.IOHprofiler_get_instance_id(), self.f.IOHprofiler_get_problem_name(),
                              self.f.IOHprofiler_get_optimization_type(), self.suite)
        self.logger = logger
    
    def clear_logger(self):
        '''Clears the logger: finishes writing info-files and closes file connections
        '''
        if self.logger is not None:
            self.logger.clear_logger()
    
    def get_target(self, raw = False):
        '''Returns the target value to reach
        
         Parameters
        ----------
        raw: 
            Whether or not to add the defined precision to the target
        Returns
        ------
        Returns the target value of the function
        '''
        target_raw = self.f.IOHprofiler_get_optimal()[0]
        if raw:
            return target_raw
        return self.precision + target_raw
    
    def reset(self):
        '''Resets the internal problem. This clears all information about previous evaluations.
        '''
        self.f.reset_problem()
        self.yopt = (self.maximization - 0.5) * -np.inf
        self.xopt = None
    
    def enable_oob_tracking(self):
        '''Enables the tracking of number of out-of-bounds points evaluated. Can be accessed trough property 'number_out_of_bounds'
        '''
        self.track_oob = True
        self.oob = 0

class custom_IOH_function(IOH_function):
    '''A wrapper to turn any python function into an IOH_function
    '''
    def __init__(self, internal_eval, fname, dim, fid = 0, iid = 0, maximization = False, suite = "No Suite", upperbound = 5, lowerbound = -5):
        '''Convert a regular function into an IOH_function, allowing for easy use with other IOHexperimenter functions, such as IOH_logger

        Parameters
        ----------
        internal_eval: 
            The function which will be evaluated. Should take only a vector as its arguments
        fname:
            The name of the problem
        dim:
            The dimension (number of variables) of the problem
        fid:
            A numerical identifier for the problem
        iid:
            The instance ID of the problem
        maximization:
            Boolean indicating whether to maximize (True) or minimize (False). Defaults to False
        suite:
            Optional, indicating which suite of functions this problem is from
        upperbound:
            The upperbound of the problem. Either one value (same for each dimension) or a vector of lenght dim
        lowerbound:
            The upperbound of the problem. Either one value (same for each dimension) or a vector of lenght dim

        '''
        self.internal_eval = internal_eval
        self.maximization = maximization
        self.yopt = (self.maximization - 0.5) * -np.inf
        self.evals = 0
        self.fname = fname
        self.dim = dim
        self.fid = fid
        self.iid = iid
        self.suite = suite
        if isinstance(upperbound, Iterable) and len(upperbound) == dim:
            self.ub = np.array(upperbound)
        else:
            self.ub = np.array(dim * [upperbound])
        if isinstance(lowerbound, Iterable) and len(lowerbound) == dim:
            self.lb = np.array(lowerbound)
        else:
            self.lb = np.array(dim * [lowerbound])
        self.logger = None
        self.y_comparison = operator.gt if self.maximization else operator.lt
        self.track_oob = False
            
    def __call__(self, x):
        y = self.internal_eval(x)
        self.evals += 1
        if self.y_comparison(y, self.yopt):
            self.xopt = x
            self.yopt = y
        if self.track_oob:
            self.oob += any(x < self.lb) or any(x > self.ub)
        if self.logger is not None:
            self.logger.process_parameters()
            self.logger.process_evaluation((self.evaluations, y, self.yopt, y, self.yopt))
        return y
    
    
    @property
    def final_target_hit(self):
        return False

    @property
    def evaluations(self):
        return self.evals
    
    @property
    def best_so_far_precision(self):
        return self.yopt
    
    @property
    def number_of_variables(self):
        return self.dim
    
    @property
    def upperbound(self):
        return self.ub
    
    @property
    def lowerbound(self):
        return self.lb
    
    def add_logger(self, logger):
        if not isinstance(logger, IOH_logger):
            raise TypeError
        logger.track_problem(self.fid, self.dim,
                              self.iid, self.fname,
                              self.maximization, self.suite)
        self.logger = logger
    
    def get_target(self, raw = False):
        return (self.maximization - 0.5) * np.inf
    
    def reset(self):
        self.yopt = (self.maximization - 0.5) * -np.inf
        self.evals = 0    
        self.xopt = None


if __name__ == '__main__':
    def random_search(func, budget = None):
        if budget is None:
            budget = int(func.number_of_variables * 1e4)

        f_opt = np.Inf
        x_opt = None

        for i in range(budget):
            x = np.random.uniform(func.lowerbound, func.upperbound)
            f = func(x)
            if f < f_opt:
                f_opt = f
                x_opt = x
        return f_opt, x_opt

    f = IOH_function(2,5,1)
    random_search(f)
    print(f.best_so_far_precision)