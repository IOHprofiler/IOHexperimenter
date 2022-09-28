import os
import unittest
import math

import ioh

DATA_DIR = os.path.join(
    os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "static")

class wmodel(ioh.problem.AbstractWModel):
    def __init__(self, instance, dim):
        super().__init__(100, instance, dim, "wmodel")

    def wmodel_evaluate(self, x) -> int:
        return x.count(1)


class TestProblem(unittest.TestCase):
    def test_get_problem(self):
        self.assertIsInstance(ioh.get_problem(1, 1, 2, "BBOB"), ioh.problem.Sphere)
        self.assertIsInstance(ioh.get_problem("Sphere", 1, 2, "BBOB"), ioh.problem.Sphere)
        self.assertIsInstance(ioh.get_problem("Sphere", 1, 2), ioh.problem.Sphere)
        self.assertIsInstance(ioh.get_problem(1, 1, 2, "PBO"), ioh.problem.OneMax)
        self.assertIsInstance(ioh.get_problem("OneMax", 1, 2, "PBO"), ioh.problem.OneMax)

    def test_wmodel(self):
        for p in map(lambda x: x(1, 10), (wmodel, ioh.problem.WModelLeadingOnes, ioh.problem.WModelOneMax)):
            self.assertEqual(p([1] * 10), 10)

    def test_enforced_bounds(self):
        p = ioh.get_problem(1, 1, 2)
        x0 = [10, 1]
        y0 = p(x0)
        p.enforce_bounds()
        y1 = p(x0)
        self.assertEqual(y0 + p.constraints.penalty(), y1)

    def test_custom_constraint(self):
        p = ioh.get_problem(1, 1, 2)
        c = ioh.RealConstraint(lambda x: float(x[0] > 1), 25.0)
        y0 = p([10, 1])
        p.add_constraint(c)
        self.assertEqual(p([10, 1]), y0 + 25)

    def test_evaluation_bbob_problems(self):
        for fid in range(1,25):
            f = ioh.get_problem(fid, 1 ,5, "BBOB")
            self.assertGreater(f([0,0,0,0,0]), -1000)

    def test_evaluation_pbo_problems(self):
        for fid in range(1,26):
            f = ioh.get_problem(fid, 1 ,4, "PBO")
            self.assertGreater(f([0,0,0,0]), -1000) 

    def test_has_submodular(self):
        problems = list(getattr(ioh.problem, "GraphProblem").problems.values())
        self.assertNotEqual(0, len(problems))
        for name in ("MaxCut", "MaxInfluence", "MaxCoverage", "PackWhileTravel"):
            self.assertNotEqual(0, len([p for p in problems if name in p]))

    def test_bbob_problems_first_instance(self):
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
        for i in sorted(ioh.problem.BBOB.problems.keys())[:24]:
            p = ioh.problem.BBOB.create(i, 1, 5)
            self.assertTrue(
                math.isclose(p([0.1, 1., 2., 4., 5.4]), expected[i-1])
            )
   
    def test_pbo_problems_first_instance(self):
        expected = [
            5.0000,
            2.0000,
            24.0000,
            3.0000,
            4.0000,
            2.0000,
            6.0000,
            4.0000,
            6.0000,
            7.0000,
            3.0000,
            2.0000,
            1.0000,
            0.0000,
            2.0000,
            1.0000,
            1.0000,
            2.5312,
            5.0000,
            6.0000,
            9.0000,
            -4.0000,
            -16.0000,
            0.45,
            -0.70717,
        ]
        for i in sorted(ioh.problem.PBO.problems.keys())[:24]:
            p = ioh.problem.PBO.create(i, 1, 9)
            y = p([1, 1, 0, 1, 0, 0, 0, 1, 1])
            self.assertTrue(math.isclose(y, expected[i-1], abs_tol = 0.000099),
                msg=f"{p} expected: {expected[i-1]} got: {y}"
            )
   
    def test_file_comparisons(self):
        for test_file in  ("pbofitness16.in", "pbofitness100.in",
                           "bbobfitness5.in", "bbobfitness20.in", ):
            with self.subTest(test_file=test_file):
                suite, dim = test_file.split("fitness")
                dim = int(dim[:-3])
                dtype = float if suite == 'bbob' else int
                tol = .01 if suite == 'bbob' else 0.000099

                with open(os.path.join(DATA_DIR, test_file)) as f:
                    for line in f:
                        fid, iid, x, y = line.split()
                        if "," in x:
                            x = x.split(",")
                        x = list(map(dtype, x))
                        p = ioh.get_problem(int(fid), int(iid), dim, suite.upper())
                        self.assertTrue(math.isclose(p(x), float(y), abs_tol = tol))



if __name__ == "__main__":
    unittest.main()
