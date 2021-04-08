import random
import unittest
import shutil
import math 

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
        self.assertIsInstance(ioh.get_problem("Sphere", 1, 2), ioh.problem.Sphere)
        self.assertIsInstance(ioh.get_problem(1, 1, 2, "PBO"), ioh.problem.OneMax)
        self.assertIsInstance(ioh.get_problem("OneMax", 1, 2, "PBO"), ioh.problem.OneMax)
    
    def test_experimenter(self):
        algorithm = Algorithm()
        exp = ioh.Experiment(
            [1, 2], [1], [5],
            njobs = -1,
            experiment_attributes = [("a", 1)],
            run_attributes = ['x'],
            logged_attributes = ['y']
        )
        exp.add_custom_problem(a_problem, "Name")
        exp(algorithm)
        shutil.rmtree("ioh_data")
        
    def test_evaluation_bbob_problems(self):
        for fid in range(1,25):
            f = ioh.get_problem(fid, 1 ,5, "BBOB")
            self.assertGreater(f([0,0,0,0,0])[0], -1000)

    def test_evaluation_pbo_problems(self):
        for fid in range(1,26):
            f = ioh.get_problem(fid, 1 ,4, "PBO")
            self.assertGreater(f([0,0,0,0])[0], -1000) 

    def test_real_fid_tests(self):
        expected = [
            161.17445568,
            12653420.412225708,
            -97.06158297486468,
            8845.53524810093,
            121.9133638725643,
            423021.00682286796,
            278.3290201933729,
            4315.032550201522,
            95168.253629631741,
            969025.0451803299,
            20911310.769634742,
            187251637.66430587,
            2198.7155122256763,
            261.60997479957,
            1103.1553337856833,
            273.87870697835791,
            35.410739487431208,
            107.64134358767826,
            -40.269198932753994,
            49739.05388887795,
            116.29585727504872,
            -914.902473409051,
            18.635078550302751,
            1782.2733296400438,
        ]
        f1 = ioh.problem.BBOB.factory()
        for i in sorted(f1.ids()):
            p = f1.create(i, 1, 5)
            assert math.isclose(p([0.1, 1., 2., 4., 5.4])[0], expected[i-1])
