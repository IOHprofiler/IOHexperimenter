import gc
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

    def test_solution(self):
        for n in range(2, 25, 2):
            val = np.random.normal(size=n)
            sol = ioh.RealSolution(val, 1)
            self.assertTrue(np.all(sol.x == val))
            self.assertTrue(np.all(ioh.RealSolution(val, 1).x == val))
            
        for n in range(2, 25, 2):
            val = np.random.randint(-100, 100, size=n)
            sol = ioh.IntegerSolution(val, 1)
            self.assertTrue(np.all(sol.x == val))
            self.assertTrue(np.all(ioh.IntegerSolution(val, 1).x == val))

    def test_solution_x_is_a_view(self):
        # Solution.x is a writable view of the solution's own values, and keeps the solution alive
        for solution, value in ((ioh.RealSolution([1.0, 2.0], 3.0), 5.5), (ioh.IntegerSolution([1, 2], 3.0), 7)):
            x = solution.x
            x[0] = value
            self.assertEqual(solution.x[0], value)
            self.assertTrue(np.shares_memory(x, solution.x))
            del solution
            gc.collect()
            self.assertEqual(list(x), [value, 2])

    def test_optimum(self):
        problem = ioh.get_problem(1, instance=1, dimension=5, problem_class=ioh.ProblemClass.PBO)
        self.assertTrue(np.all(np.ones(5) == problem.optimum.x))
        
    
    def test_constraint(self):
        def cons(x):
            """If x[1] == 1, the constraint is violated."""
            v = float(x[1] == 1)
            return v
        
        c = ioh.RealConstraint(cons, 25.0)
        self.assertFalse(c.is_feasible([0, 1]))
        self.assertTrue(c.is_feasible([1, 0]))

    def test_bounds(self):
        bounds = ioh.RealBounds([-1, 0], [1, 2])

        self.assertTrue(np.all(bounds.lb == np.array([-1, 0])))
        self.assertTrue(np.all(bounds.ub == np.array([1, 2])))

    def test_copied_arrays(self):
        suite = ioh.suite.BBOB(range(1, 5))
        self.assertListEqual(list(suite.problem_ids), list(range(1, 5)))

