import unittest

import ioh


def problem(x):
    return sum(x)


def zero(x):
    return 0.0


def tx(x, iid):
    x[1] = iid
    return x


def ty(y, iid):
    return y * iid


def co(iid, dim):
    return [iid] * dim, iid


class TestWrappedProblem(unittest.TestCase):
    def test_wrap_problem(self):
        p = ioh.wrap_problem(
            problem,
            "problem",
            ioh.ProblemClass.REAL,
            dimension=2,
            transform_variables=tx,
            transform_objectives=ty,
            calculate_objective=co,
        )
        self.assertEqual(p([1, 2]), 2.0)
        self.assertEqual(p.optimum.x[0], 1)
        self.assertEqual(p.optimum.x[0], p.optimum.y)

    # @unittest.skip("can cause a segfault (cpp exit handler)")
    def test_wrap_problem_scoped(self):
        def w():
            ioh.problem.wrap_real_problem(lambda _: 0.0, "l")
            return ioh.get_problem("l")
        p = w()
        y = p([0]*5)
        self.assertEqual(y, 0.0)

    def test_wrap_problem_lambda(self):
        def func(_):
            return 0.0

        ioh.problem.wrap_real_problem(func, "f")
        p = ioh.get_problem("f")
        p([0] * 5)

        ioh.problem.wrap_real_problem(func, "f")
        p2 = ioh.get_problem("f")
        self.assertEqual(p.meta_data.problem_id, p2.meta_data.problem_id)
        self.assertEqual(p.meta_data.name, p2.meta_data.name)

    def test_wrap_problem_builtins(self):
        for f in (sum, min, max):
            ioh.problem.wrap_real_problem(f, f.__name__)
            p = ioh.get_problem(f.__name__)
            y = p([0]*5)
            self.assertEqual(y, 0.0)

    def test_wrap_problem_constrains(self):
        c = ioh.RealConstraint(lambda x: float(x[0] > 1), 10.0)
        p = ioh.wrap_problem(
            zero,
            "test",
            ioh.ProblemClass.REAL,
            dimension=2,
            constraints=[c],
        )
        self.assertEqual(p([10, 0]), 10.0)
        p.remove_constraint(c)
        self.assertEqual(p([10, 0]), 0)

    def test_wrap_problem_bounds(self):
        p = ioh.wrap_problem(
            zero,
            "bounds",
            ioh.ProblemClass.REAL,
            dimension=3,
            lb=-5,
            ub=5,
        )
        self.assertListEqual(p.bounds.lb.tolist(), [-5] * 3)
        self.assertListEqual(p.bounds.ub.tolist(), [5] * 3)

    def test_wrap_integer_problem(self):
        def ip(x):
            return sum(x)

        p = ioh.wrap_problem(
            ip,
            "int_problem",
            ioh.ProblemClass.INTEGER,
            dimension=2,
            lb=-1,
            ub=1,
        )
        self.assertEqual(p([0, 0]), 0)
        self.assertEqual(p.state.evaluations, 1)
        self.assertIsInstance(p.bounds, ioh.IntegerBounds)
        self.assertListEqual(p.bounds.lb.tolist(), [-1] * 2)
        self.assertListEqual(p.bounds.ub.tolist(), [1] * 2)

    def test_wrap_problem_solution_object(self):
        def co_sol(iid, dim):
            return ioh.RealSolution([iid] * dim, float(iid))

        p = ioh.wrap_problem(
            zero,
            "opt_problem",
            ioh.ProblemClass.REAL,
            dimension=2,
            calculate_objective=co_sol,
        )
        self.assertListEqual(p.optimum.x.tolist(), [1, 1])
        self.assertEqual(p.optimum.y, 1.0)


if __name__ == "__main__":
    unittest.main()
