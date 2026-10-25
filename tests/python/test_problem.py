import os
import subprocess
import sys
import tempfile
import textwrap
import unittest
import math

import ioh
import numpy as np


# the data files of the tests, in tests/data
DATA_DIR = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "data")


class wmodel(ioh.problem.AbstractWModel):
    def __init__(self, instance, dim):
        super().__init__(100, instance, dim, "wmodel")

    def wmodel_evaluate(self, x) -> int:
        return x.count(1)


class TestProblem(unittest.TestCase):
    def test_get_problem(self):
        self.assertIsInstance(
            ioh.get_problem(1, 1, 2, ioh.ProblemClass.BBOB), ioh.problem.Sphere
        )
        self.assertIsInstance(
            ioh.get_problem("Sphere", 1, 2, ioh.ProblemClass.BBOB), ioh.problem.Sphere
        )
        self.assertIsInstance(ioh.get_problem("Sphere", 1, 2), ioh.problem.Sphere)
        self.assertIsInstance(
            ioh.get_problem(1, 1, 2, ioh.ProblemClass.PBO), ioh.problem.OneMax
        )
        self.assertIsInstance(
            ioh.get_problem("OneMax", 1, 2, ioh.ProblemClass.PBO), ioh.problem.OneMax
        )

    def test_vectorized_eval(self):
        n_samples = 5
        dim = 2

        problem = ioh.get_problem(1, 1, dim)
        X = np.zeros((n_samples, dim))
        y = problem(X)
        self.assertIsInstance(y, list)
        self.assertEqual(len(y), n_samples)
        self.assertEqual(problem.state.evaluations, n_samples)
        self.assertSetEqual(set(y), {80.88209408})

    def test_wmodel(self):
        for p in map(
            lambda x: x(1, 10),
            (wmodel, ioh.problem.WModelLeadingOnes, ioh.problem.WModelOneMax),
        ):
            self.assertEqual(p([1] * 10), 10)

    def test_can_access_optimization_type(self):
        p = ioh.get_problem(1, 1, 2)
        self.assertTrue(p.meta_data.optimization_type is not None)


    def test_class_based_problem(self):
        class A(ioh.problem.RealSingleObjective):
            def __init__(self, instance, dim) -> None:
                super().__init__(
                    "A",
                    dim,
                    instance,
                    False,
                    ioh.RealBounds(dim, 10, 20),
                    [], 
                    ioh.RealSolution([1] * dim, dim)
                )
        a = A(1, 2)
        try:
            a([0] * 2)
            self.fail()
        except RuntimeError:
            pass

        setattr(a, "evaluate", lambda x:sum(xi == 1 for xi in x))
        self.assertEqual(a([0, 0]), 0)
        self.assertEqual(a([0, 1]), 1)
        self.assertFalse(a.state.optimum_found)
        self.assertEqual(a([1, 1]), 2)
        self.assertTrue(a.state.optimum_found)

    def test_real_star_discrepancy(self):
        uniform = ioh.get_problem(
            30,
            4,
            2,
        )
        self.assertAlmostEqual(uniform([0.9, 0.5]), 0.25, 3)
        sobol = ioh.get_problem(40, 4, 2)
        self.assertAlmostEqual(sobol([0.9, 0.5]), 0.0499, 3)
        halton = ioh.get_problem(50, 4, 2)
        self.assertAlmostEqual(halton([0.9, 0.5]), 0.1499, 3)

        random = ioh.problem.RealStarDiscrepancy(
            instance=69,
            n_variables=2,
            n_samples=9,
            sampler_type=ioh.problem.StarDiscrepancySampler.HALTON,
        )
        self.assertEqual(random([10, 10]), -float("inf"))

        self.assertAlmostEqual(random([0.9, 0.5]), 0.0055, 3)

        self.assertTrue(math.isnan(uniform.optimum.x[0]))
        self.assertTrue(math.isinf(uniform.optimum.y))

    def test_integer_star_discrepancy(self):
        uniform = ioh.get_problem(30, 4, 2, problem_class=ioh.ProblemClass.STAR_INTEGER)
        self.assertAlmostEqual(uniform([9, 9]), 0.11825, 3)

        sobol = ioh.get_problem(40, 4, 2, problem_class=ioh.ProblemClass.STAR_INTEGER)
        self.assertAlmostEqual(sobol([9, 9]), 0.1797, 3)

        halton = ioh.get_problem(50, 4, 2, problem_class=ioh.ProblemClass.STAR_INTEGER)
        self.assertAlmostEqual(halton([9, 9]), 0.2222, 3)

        random = ioh.problem.IntegerStarDiscrepancy(
            instance=69,
            n_variables=2,
            n_samples=10,
            sampler_type=ioh.problem.StarDiscrepancySampler.HALTON,
        )
        self.assertEqual(random([10, 10]), 0)
        self.assertEqual(random([11, 11]), -float("inf"))

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
        for fid in range(1, 25):
            f = ioh.get_problem(fid, 1, 5, ioh.ProblemClass.BBOB)
            self.assertGreater(f([0, 0, 0, 0, 0]), -1000)

    def test_evaluation_pbo_problems(self):
        for fid in range(1, 26):
            f = ioh.get_problem(fid, 1, 4, ioh.ProblemClass.PBO)
            self.assertGreater(f([0, 0, 0, 0]), -1000)

    def test_has_submodular(self):
        problems = list(getattr(ioh.problem, "GraphProblem").problems.values())
        self.assertNotEqual(0, len(problems))
        for name in ("MaxCut", "MaxInfluence", "MaxCoverage", "PackWhileTravel"):
            self.assertNotEqual(0, len([p for p in problems if name in p]))

    def test_load_graph_instances_from_file(self):
        # in a fresh interpreter, because the loaded graphs stay registered
        code = textwrap.dedent("""
            import os, tempfile, warnings
            import ioh

            with tempfile.TemporaryDirectory() as d:
                os.mkdir(os.path.join(d, "graphs"))
                with open(os.path.join(d, "graphs", "triangle"), "w") as f:
                    f.write("0\\n1 2\\n2 3\\n3 1\\n")
                definitions = os.path.join(d, "definitions")
                with open(definitions, "w") as f:
                    f.write("graphs/triangle|NULL|NULL|NULL\\n")

                # the graph is added after the built-in graphs (the last is 2308), which stay as they are
                builtins = dict(ioh.problem.GraphProblem.problems)
                added = ioh.problem.MaxCut.load_instances(definitions)
                assert added == {2309: "MaxCut2309"}, added
                assert dict(ioh.problem.GraphProblem.problems) == {**builtins, **added}
                p = ioh.get_problem(2309, 1, 1, ioh.ProblemClass.GRAPH)
                assert (p.meta_data.problem_id, p.meta_data.name, p.meta_data.n_variables) == (2309, "MaxCut2309", 3)
                p = ioh.get_problem("MaxCut2309", 1, 1, ioh.ProblemClass.INTEGER)
                assert (p.meta_data.problem_id, p.meta_data.n_variables) == (2309, 3)
                assert ioh.get_problem(2000, 1, 1, ioh.ProblemClass.GRAPH).meta_data.n_variables == 800

                # loading it again changes nothing; without a path it is deprecated and does nothing
                assert ioh.problem.MaxCut.load_instances(definitions) == added
                with warnings.catch_warnings(record=True) as caught:
                    warnings.simplefilter("always")
                    ioh.problem.MaxCut.load_instances()
                assert [w.category for w in caught] == [DeprecationWarning], caught
                assert dict(ioh.problem.GraphProblem.problems) == {**builtins, **added}
                try:
                    ioh.problem.MaxCut.load_instances(os.path.join(d, "missing"))
                    raise AssertionError("no error for a missing file")
                except ValueError:
                    pass
            """)
        env = dict(os.environ, PYTHONPATH=os.pathsep.join(sys.path))
        result = subprocess.run([sys.executable, "-c", code], capture_output=True, text=True, env=env)
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_missing_data(self):
        # with an empty data directory: registering needs no data, and creating a problem that needs data raises
        code = textwrap.dedent("""
            import ioh

            assert len(ioh.problem.GraphProblem.problems) == 78
            assert ioh.get_problem(1, 1, 2).meta_data.name == "Sphere"
            assert ioh.get_problem(1101, 1, 1, ioh.ProblemClass.CEC2013).meta_data.n_variables == 1
            for pid, problem_class in ((2000, ioh.ProblemClass.GRAPH), (1001, ioh.ProblemClass.CEC2022),
                                       (1111, ioh.ProblemClass.CEC2013)):
                try:
                    ioh.get_problem(pid, 1, 10, problem_class)
                    raise AssertionError(f"no error for {pid}")
                except RuntimeError as error:
                    assert "data version" in str(error), error
            """)
        with tempfile.TemporaryDirectory() as empty:
            env = dict(os.environ, PYTHONPATH=os.pathsep.join(sys.path), IOH_RESOURCES=empty)
            result = subprocess.run([sys.executable, "-c", code], capture_output=True, text=True, env=env)
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_subclass_ids(self):
        # a problem defined by subclassing gets the first free id of its base class's factory, like wrap_problem
        def subclass(name):
            class Subclass(ioh.problem.IntegerSingleObjective):
                def __init__(self, instance=1, n_variables=2):
                    super().__init__(name, n_variables, instance, True, ioh.IntegerBounds(n_variables, 0, 1))

                def evaluate(self, x):
                    return 0.0

            return Subclass

        first, second = subclass("TestSubclassIdsA")(), subclass("TestSubclassIdsB")()
        ids = {name: pid for pid, name in ioh.problem.IntegerSingleObjective.problems.items()}
        self.assertEqual(first.meta_data.problem_id, ids["TestSubclassIdsA"])
        self.assertEqual(second.meta_data.problem_id, ids["TestSubclassIdsB"])
        self.assertGreater(first.meta_data.problem_id, 10004)
        self.assertNotEqual(first.meta_data.problem_id, second.meta_data.problem_id)
        self.assertEqual(subclass("TestSubclassIdsA")().meta_data.problem_id, first.meta_data.problem_id)
        created = ioh.get_problem("TestSubclassIdsB", 1, 3, ioh.ProblemClass.INTEGER)
        self.assertEqual(created.meta_data.problem_id, second.meta_data.problem_id)

    def test_builtin_names_cannot_be_registered(self):
        with self.assertRaises(ValueError):
            ioh.wrap_problem(lambda x: 0.0, "Sphere")

        class Sphere(ioh.problem.RealSingleObjective):
            def __init__(self):
                super().__init__("Sphere", 2, 1, True, ioh.RealBounds(2, -1, 1))

            def evaluate(self, x):
                return 0.0

        with self.assertRaises(ValueError):
            Sphere()
        self.assertIsInstance(ioh.get_problem(1, 1, 2), ioh.problem.Sphere)

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
            self.assertTrue(math.isclose(p([0.1, 1.0, 2.0, 4.0, 5.4]), expected[i - 1]))

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
            self.assertTrue(
                math.isclose(y, expected[i - 1], abs_tol=0.000099),
                msg=f"{p} expected: {expected[i-1]} got: {y}",
            )

    def test_file_comparisons(self):
        for test_file in (
            "pbofitness16.in",
            "pbofitness100.in",
            "bbobfitness5.in",
            "bbobfitness20.in",
            "cec_problem2013.in",
            "cec_problem2022.in",
        ):
            with self.subTest(test_file=test_file):
                is_integer = 'pbo' in test_file
                problem_class = (
                    ioh.ProblemClass.INTEGER if is_integer else ioh.ProblemClass.REAL
                )
                dtype = int if is_integer else float
                tol = 0.000099 if is_integer else 0.01

                with open(os.path.join(DATA_DIR, test_file)) as f:
                    for line in f:
                        fid, iid, x, y = line.split()
                        if "," in x:
                            x = x.split(",")

                        if "." in x:
                            x = [float(x)]
                            dim = 1
                        else:
                            x = list(map(dtype, x))
                            dim = len(x)
                        p = ioh.get_problem(int(fid), int(iid), dim, problem_class)
                        self.assertTrue(math.isclose(p(x), float(y), abs_tol=tol))

    def test_sbox(self):
        for pid, name in ioh.problem.SBOX.problems.items():
            sbox = ioh.get_problem(pid, 1, 4, ioh.ProblemClass.SBOX)
            bbob = ioh.get_problem(pid, 1, 4, ioh.ProblemClass.BBOB)
            self.assertEqual(sbox.constraints.n(), 1)
            self.assertEqual(sbox([10, 10, 10, 10]), float("inf"))
            self.assertNotEqual(bbob([10, 10, 10, 10]), float("inf"))
            self.assertAlmostEqual(bbob.optimum.y, sbox.optimum.y)

            if pid in (5, 9, 19, 20):
                continue
            self.assertFalse(all(bbob.optimum.x == sbox.optimum.y))

    def test_sbox_suite(self):
        suite = ioh.suite.SBOX(range(25))
        self.assertEqual(len(suite), 24)


if __name__ == "__main__":
    unittest.main()
