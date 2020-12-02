import os
import shutil
import uuid
import unittest
import numpy as np


class TmpdirTestCase(unittest.TestCase):    
    def setUp(self):
        self.tmpdir = os.path.join(os.getcwd(), str(uuid.uuid1()))

    def tearDown(self):
        shutil.rmtree(self.tmpdir)


class opt_alg:
    def __init__(self, budget):
        self.budget = budget
        self.f_opt = np.Inf
        self.x_opt = None

    def __call__(self, func):
        for _ in range(self.budget):
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