import random
import unittest

import ioh

class Algorithm:
    x = 10
    y = 1

    def __call__(self, p: ioh.problem.Real):
        for i in range(10000):
            x = list(map(lambda x: random.random(), range(p.meta_data.n_variables)))    
            p(x)
            y = i

def a_problem(x):
    return [0]

class TestProblem(unittest.TestCase):
    def test_get_problem(self): 
        self.assertIsInstance(ioh.get_problem(1, 1, 2, "BBOB"), ioh.problem.Sphere)
        self.assertIsInstance(ioh.get_problem("Sphere", 1, 2, "BBOB"), ioh.problem.Sphere)
        self.assertIsInstance(ioh.get_problem(1, 1, 2, "PBO"), ioh.problem.OneMax)
        self.assertIsInstance(ioh.get_problem("OneMax", 1, 2, "PBO"), ioh.problem.OneMax)
    
    def test_experimenter(self):
        algorithm = Algorithm()
        exp = ioh.Experiment(
            [1, 2], [1], [5],
            njobs = 2,
            experiment_attributes = [("a", 1)],
            run_attributes = ['x'],
            logged_attributes = ['y']
        )
        exp.add_custom_problem(a_problem, "Name")
        exp(algorithm)
