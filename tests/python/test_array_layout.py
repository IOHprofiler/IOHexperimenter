import ioh
import unittest

import numpy as np


def tester_inc_one_transform(x, i):
    return x+1

class TestArrayLayout(unittest.TestCase):
    def test_wrap_problem(self):
        def make_tester(values):
            def tester(x):
                self.assertTrue(np.all(x == values), f"Expected: {values}, got: {x}")
                return sum(x)
            return tester

        values = [
            [0, 0],
            [1, 1],
            [1, 0],
            [0, 1],
        ]
        for v in values:
            f = make_tester(v)
            p = ioh.wrap_problem(f, dimension=len(v))
            y = p(v)
            self.assertEqual(y, sum(v))


    def test_transform_variables(self):
        def make_tester(values):
            def tester(x):
                self.assertTrue(np.all(x-1 == values), f"Expected: {values}, got: {x}")
                return sum(x)
            return tester

        values = [
            np.array([0, 0]),
            np.array([1, 1]),
            np.array([1, 0]),
            np.array([0, 1]),
        ]
        for v in values:
            f = make_tester(v)
            p = ioh.wrap_problem(f, dimension=len(v), transform_variables=tester_inc_one_transform)
            y = p(v)
            self.assertEqual(y, sum(v+1))

            
