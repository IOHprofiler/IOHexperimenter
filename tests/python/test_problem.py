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

#    def test_experimenter(self):
#        algorithm = Algorithm()
#        exp = ioh.Experiment(
#            [1, 2], [1], [5],
#            njobs = -1,
#            experiment_attributes = [("a", 1)],
#            run_attributes = ['x'],
#            logged_attributes = ['y']
#        )
#        exp.add_custom_problem(a_problem, "Name")
#        exp(algorithm)
#        shutil.rmtree("ioh_data")
#        
        
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
   
    
    def test_integer_problems(self):
        def eval(p,x,y) :
            assert math.isclose(p(x)[0] , y,abs_tol = 0.000099)
        x = [1, 1, 0, 1, 0, 0, 0, 1, 1]
        eval(ioh.problem.OneMax(1, 9), x, 5.0000)
        eval(ioh.problem.OneMaxDummy1(1, 9), x, 3.0000)
        eval(ioh.problem.OneMaxDummy2(1, 9), x, 4.0000)
        eval(ioh.problem.OneMaxEpistasis(1, 9), x, 6.0000)
        eval(ioh.problem.OneMaxNeutrality(1, 9), x, 2.0000)
        eval(ioh.problem.OneMaxRuggedness1(1, 9), x, 4.0000)
        eval(ioh.problem.OneMaxRuggedness2(1, 9), x, 6.0000)
        eval(ioh.problem.OneMaxRuggedness3(1, 9), x, 7.0000)
        eval(ioh.problem.LeadingOnes(1, 9), x, 2.0000)
        eval(ioh.problem.LeadingOnesDummy1(1, 9), x, 3.0000)
        eval(ioh.problem.LeadingOnesDummy2(1, 9), x, 2.0000)
        eval(ioh.problem.LeadingOnesEpistasis(1, 9), x, 0.0000)
        eval(ioh.problem.LeadingOnesNeutrality(1, 9), x, 1.0000)
        eval(ioh.problem.LeadingOnesRuggedness1(1, 9), x, 2.0000)
        eval(ioh.problem.LeadingOnesRuggedness2(1, 9), x, 1.0000)
        eval(ioh.problem.LeadingOnesRuggedness3(1, 9), x, 1.0000)
        eval(ioh.problem.Linear(1, 9), x, 24.0000)
        eval(ioh.problem.MIS(1, 9), x, -4.0000)
        eval(ioh.problem.LABS(1, 9), x, 2.5312)
        eval(ioh.problem.NQueens(1, 9), x, -16.0000)
        eval(ioh.problem.IsingRing(1, 9), x, 5.0000)
        eval(ioh.problem.IsingTorus(1, 9), x, 6.0000)
        eval(ioh.problem.IsingTriangular(1, 9), x, 9.0000)

    def test_pbo_suite_100D(self):
        def eval(p,x,y) :
            assert math.isclose(p(x)[0] , y,abs_tol = 0.000099)
        filename = "tests/python/pbofitness100.in"
        f = open(filename,'r')
        line = f.readline()
        while line  :
            f_id,ins_id,x_str,y = line.split()
            x = []
            for i in range(len(x_str)) :
                x.append(int(x_str[i]))
            p = ioh.get_problem(int(f_id), int(ins_id), 100, "PBO")
            eval(p,x,float(y))
            line = f.readline()
    
    def test_pbo_suite_16D(self):
        def eval(p,x,y) :
            assert math.isclose(p(x)[0] , y,abs_tol = 0.000099)
        filename = "tests/python/pbofitness16.in"
        f = open(filename,'r')
        line = f.readline()
        while line  :
            f_id,ins_id,x_str,y = line.split()
            x = []
            for i in range(len(x_str)) :
                x.append(int(x_str[i]))
            p = ioh.get_problem(int(f_id), int(ins_id), 16, "PBO")
            eval(p,x,float(y))
            line = f.readline()
            
    def test_bbob_suite_5D(self):
        def eval(p,x,y) :
            assert math.isclose(p(x)[0] , y,abs_tol = 0.01)
        filename = "tests/python/bbobfitness5.in"
        f = open(filename,'r')
        line = f.readline()
        while line:
            f_id,ins_id,x_str,y = line.split()
            x_str = x_str.split(',')
            x = []
            for i in x_str :
                x.append(float(i))
            p = ioh.get_problem(int(f_id), int(ins_id), 5, "BBOB")
            eval(p,x,float(y))
            line = f.readline()

    def test_bbob_suite_20D(self):
        def eval(p,x,y) :
            assert math.isclose(p(x)[0] , y,abs_tol = 0.01)
        filename = "tests/python/bbobfitness20.in"
        f = open(filename,'r')
        line = f.readline()
        while line  :
            f_id,ins_id,x_str,y = line.split()
            x_str = x_str.split(',')
            x = []
            for i in x_str :
                x.append(float(i))
            p = ioh.get_problem(int(f_id), int(ins_id), 20, "BBOB")
            eval(p,x,float(y))
            line = f.readline()
