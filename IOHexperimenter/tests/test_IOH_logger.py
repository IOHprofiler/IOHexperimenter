import pytest

import numpy as np
from os import path
from .. import IOH_function, custom_IOH_function, IOH_logger

def test_initialization_logger(tmpdir):
    l = IOH_logger(tmpdir.dirname, "temp_run")
    l.clear_logger()
    assert 1
    
def test_logging_fct(tmpdir):
    f = IOH_function(1, 5, 1, suite = "BBOB")
    l = IOH_logger(tmpdir.dirname, "temp_run")
    f.add_logger(l)
    f([0] * 5)
    assert path.exists(f"{tmpdir.dirname}/temp_run")
    f.clear_logger()

class opt_alg:
    def __init__(self, budget):
        self.budget = budget
        self.f_opt = np.Inf
        self.x_opt = None

    def __call__(self, func):
        for i in range(self.budget):
            x = np.random.uniform(func.lowerbound, func.upperbound)
            f = func(x)
            if f < self.f_opt:
                self.f_opt = f
                self.x_opt = x
        return self.f_opt, self.x_opt
    
    @property
    def param_rate(self):
        return np.random.randint(100)
    
    @property
    def static_param(self):
        return 123
    
    @property
    def dynamic_prop(self):
        return np.random.randint(10)
    
    
def test_logging_with_params(tmpdir):
    f = IOH_function(1, 5, 1, suite = "BBOB")
    l = IOH_logger(tmpdir.dirname, "temp_run")
    a = opt_alg(10000)
    l.track_parameters(a, 'param_rate')
    l.set_static_attributes('static_param')
    l.set_dynamic_attributes('dynamic_prop')
    f.add_logger(l)
    a(f)
    assert path.exists(f"{tmpdir.dirname}/temp_run")
    f.clear_logger()
