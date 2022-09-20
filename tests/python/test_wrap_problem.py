import unittest

import ioh


def problem(x):
    return sum(x)

def zero(x):
    return 0.


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
            "Real",
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
        l = lambda _: 0.0
        ioh.problem.wrap_real_problem(l, "f")
        p = ioh.get_problem("f")
        p([0]*5)

        ioh.problem.wrap_real_problem(l, "f")
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
        p = ioh.wrap_problem(zero, "test", "Real", dimension = 2, constraints=[c])
        self.assertEqual(p([10, 0]), 10.0)
        p.remove_constraint(c)
        self.assertEqual(p([10, 0]), 0)


if __name__ == "__main__":
    unittest.main()
