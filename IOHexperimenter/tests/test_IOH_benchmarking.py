import pytest
import numpy as np
from .. import IOHexperimenter, IOH_function, custom_IOH_function

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
    
def test_initialization_PBO(tmpdir):
    exp = IOHexperimenter()
    exp.initialize_BBOB([1,2], [5], [5], 2, 1e-8)
    exp.set_logger_location(tmpdir.dirname, "run")
    a = opt_alg(1000)
    exp([a])
    