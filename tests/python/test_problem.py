import unittest
import ioh

class TestProblem(unittest.TestCase):
    def test_get_problem(self):

        self.assertIsInstance(ioh.get_problem(1, 1, 2, "BBOB"), ioh.problem.Sphere)
        self.assertIsInstance(ioh.get_problem("Sphere", 1, 2, "BBOB"), ioh.problem.Sphere)
        self.assertIsInstance(ioh.get_problem("Sphere", 1, 2), ioh.problem.Sphere)
        self.assertIsInstance(ioh.get_problem(1, 1, 2, "PBO"), ioh.problem.OneMax)
        self.assertIsInstance(ioh.get_problem("OneMax", 1, 2, "PBO"), ioh.problem.OneMax)
        self.assertIsInstance(ioh.get_problem("OneMax", 1, 2), ioh.problem.OneMax)
        
    def test_evaluation_bbob_problems(self):
        for fid in range(1,25):
            f = ioh.get_problem(fid, 1 ,5, "BBOB")
            self.assertGreater(f([0,0,0,0,0])[0], -1000)

    def test_evaluation_pbo_problems(self):
        for fid in range(1,26):
            f = ioh.get_problem(fid, 1 ,4, "PBO")
            self.assertGreater(f([0,0,0,0])[0], -1000) 