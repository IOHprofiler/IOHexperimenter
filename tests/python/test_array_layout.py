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

    def test_transform_variables_copy(self):
        # transform_variables gets x as a new array, which it may keep; for an integer problem it may return an array
        # of any integer or bool type
        kept = []

        def transform(x, _):
            kept.append(x)
            return x == 0

        p = ioh.wrap_problem(lambda x: float(np.sum(x)), "TestArrayLayoutTransformCopy", ioh.ProblemClass.INTEGER, 3,
                             transform_variables=transform)
        self.assertEqual(p([1, 0, 1]), 1.0)
        self.assertEqual(p([0, 0, 0]), 3.0)
        gc.collect()
        self.assertEqual([x.dtype for x in kept], [np.int32] * 2)
        self.assertEqual([x.tolist() for x in kept], [[1, 0, 1], [0, 0, 0]])

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
            # assigning as many values keeps the solution's buffer, so the view shows them
            solution.x = np.array([4, 2])
            self.assertEqual(list(x), [4, 2])
            x[0] = value
            del solution
            gc.collect()
            self.assertEqual(list(x), [value, 2])

    def test_setters_take_arrays(self):
        # x, lb and ub take arrays at once (an array of another type is converted for reals), and any sequence
        solution = ioh.RealSolution([0.0, 0.0], 1.0)
        bounds = ioh.RealBounds([0.0, 0.0], [1.0, 1.0])
        for value in (np.array([1.5, 2.5]), np.array([1, 2], dtype=np.int32), np.arange(4.0)[::2], [1.5, 2.5],
                      (1, 2), range(2)):
            solution.x = value
            bounds.lb = value
            bounds.ub = value
            for values in (solution.x, bounds.lb, bounds.ub):
                self.assertEqual(values.dtype, np.float64)
                self.assertEqual(values.tolist(), list(value))
        solution.x = solution.x + 1
        self.assertEqual(solution.x.tolist(), [1.0, 2.0])

    def test_returned_arrays(self):
        # the values that a problem returns as copies are new writable arrays on every read
        problem = ioh.problem.ManyAffine(1, 3)
        for name, dtype in (("weights", np.float64), ("instances", np.int32), ("scale_factors", np.float64),
                            ("function_values", np.float64)):
            values = getattr(problem, name)
            self.assertIsInstance(values, np.ndarray, name)
            self.assertEqual((values.dtype, values.shape), (dtype, (24,)), name)
            self.assertTrue(values.flags.writeable, name)
            self.assertFalse(np.shares_memory(values, getattr(problem, name)), name)
        grid = ioh.get_problem(30, 1, 3, ioh.ProblemClass.STAR_REAL).grid
        self.assertEqual((grid.dtype, grid.shape), (np.float64, (10, 3)))
        self.assertTrue(grid.flags.writeable)

    def test_log_info(self):
        # x, violations and penalties are new float64 arrays, also for integer problems: snapshots, which later
        # evaluations do not change (problem.log_info is the problem's own log info)
        for problem, x in ((ioh.get_problem(1, 1, 3), [1.0, 2.0, 3.0]),
                           (ioh.get_problem(1, 1, 3, ioh.ProblemClass.PBO), [1, 0, 1])):
            problem(x)
            info = problem.log_info
            kept = info.x
            for values in (kept, info.violations, info.penalties):
                self.assertIsInstance(values, np.ndarray)
                self.assertEqual(values.dtype, np.float64)
                self.assertTrue(values.flags.writeable)
            self.assertEqual(kept.shape, (3,))
            self.assertEqual((info.violations.tolist(), info.penalties.tolist()), ([0.0], [0.0]))
            problem([0, 0, 0])
            self.assertEqual(kept.tolist(), x)
            self.assertEqual(info.x.tolist(), [0, 0, 0])

    def test_log_info_objective(self):
        # objective is a copy of the log info's optimum: it stays valid when adding a constraint replaces the log
        # info's vectors, and writing to it does not change the problem's
        problem = ioh.get_problem(1, 1, 3)
        objective = problem.log_info.objective
        x = objective.x
        expected = x.tolist()
        problem.add_constraint(ioh.RealConstraint(lambda _: 0.0))
        problem.enforce_bounds()
        x[0] = 100.0
        self.assertEqual(objective.x.tolist(), [100.0] + expected[1:])
        self.assertEqual(problem.log_info.objective.x.tolist(), expected)
        self.assertEqual(problem.log_info.violations.shape, (3,))

    def test_cec2013_optima(self):
        # a list of copies of the problem's optima: writing to them does not change the problem's
        problem = ioh.get_problem(1101, 1, 1, ioh.ProblemClass.CEC2013)
        optima = problem.optima
        self.assertIsInstance(optima, list)
        self.assertIsInstance(optima[0], ioh.RealSolution)
        expected = [optimum.x.tolist() for optimum in optima]
        optima[0].x[0] = 0.123
        self.assertEqual([optimum.x.tolist() for optimum in problem.optima], expected)

    def test_graph_weights(self):
        # read-only views of the graph's data, kept alive by the graph object (also when the problem is gone)
        problem = ioh.get_problem(2000, 1, 1, ioh.ProblemClass.GRAPH)
        graph = problem.graph
        weights = [graph.edge_weights, graph.vertex_weights, graph.constraint_weights, graph.constraint_variances]
        for values in weights:
            self.assertIsInstance(values, np.ndarray)
            self.assertEqual(values.dtype, np.float64)
            self.assertFalse(values.flags.writeable)
        self.assertEqual(len(weights[0]), 19176)
        self.assertTrue(np.shares_memory(weights[0], graph.edge_weights))
        with self.assertRaises(ValueError):
            weights[0][0] = 1.0
        expected = [values.tolist() for values in weights]
        del problem, graph
        gc.collect()
        self.assertEqual([values.tolist() for values in weights], expected)

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
        for values in (suite.problem_ids, suite.instances, suite.dimensions):
            self.assertIsInstance(values, np.ndarray)
            self.assertEqual(values.dtype, np.int32)
            self.assertTrue(values.flags.writeable)
        self.assertListEqual(list(ioh.suite.BBOB(suite.problem_ids, suite.instances).problem_ids), list(range(1, 5)))

