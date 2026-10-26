"""Test the package from several threads at once; on a free-threaded Python (3.14t and later), without the GIL.

Different objects may be used from different threads at the same time (T6.6); one problem, logger or suite is used by
one thread at a time.
"""

import os
import subprocess
import sys
import sysconfig
import tempfile
import textwrap
import threading
import unittest
from concurrent.futures import ThreadPoolExecutor

import numpy as np

import ioh

THREADS = 8


def in_threads(work, n=THREADS):
    """Run work(i) for i in range(n), each in its own thread, all starting together; return their results."""
    barrier = threading.Barrier(n)

    def start(i):
        barrier.wait()
        return work(i)

    with ThreadPoolExecutor(n) as pool:
        return list(pool.map(start, range(n)))


# problems of every family, as (id, instance, dimension, ProblemClass); the graph problems 2300-2302 share a graph file
CASES = [
    (1, 1, 5, ioh.ProblemClass.BBOB),
    (8, 2, 10, ioh.ProblemClass.BBOB),
    (21, 3, 5, ioh.ProblemClass.BBOB),
    (7, 1, 5, ioh.ProblemClass.SBOX),
    (1, 1, 32, ioh.ProblemClass.PBO),
    (25, 1, 32, ioh.ProblemClass.PBO),
    (1001, 1, 10, ioh.ProblemClass.CEC2022),
    (1012, 1, 10, ioh.ProblemClass.CEC2022),
    (1101, 1, 1, ioh.ProblemClass.CEC2013),
    (1115, 1, 1, ioh.ProblemClass.CEC2013),
    (2000, 1, 1, ioh.ProblemClass.GRAPH),
    (2100, 1, 1, ioh.ProblemClass.GRAPH),
    (2300, 1, 1, ioh.ProblemClass.GRAPH),
    (2301, 1, 1, ioh.ProblemClass.GRAPH),
    (2302, 1, 1, ioh.ProblemClass.GRAPH),
    (30, 1, 3, ioh.ProblemClass.REAL),
]


def points(case, n=50):
    """Search points of a problem, the same in every thread."""
    problem = ioh.get_problem(*case)
    rng = np.random.default_rng(case[0])
    if isinstance(problem, ioh.iohcpp.problem.IntegerSingleObjective):
        return rng.integers(0, 2, (n, problem.meta_data.n_variables))
    return rng.uniform(problem.bounds.lb, problem.bounds.ub, (n, problem.meta_data.n_variables))


class TestThreads(unittest.TestCase):
    def test_gil_stays_disabled(self):
        if not sysconfig.get_config_var("Py_GIL_DISABLED"):
            self.skipTest("not a free-threaded Python")
        self.assertFalse(sys._is_gil_enabled())

    def test_evaluate(self):
        # each thread creates and evaluates its own problems; the results are those of a single thread
        inputs = {case: points(case) for case in CASES}

        def evaluate(_):
            return {case: [ioh.get_problem(*case)(x) for x in inputs[case]] for case in CASES}

        expected = evaluate(None)
        for result in in_threads(evaluate):
            self.assertEqual(result, expected)

    def test_max_influence(self):
        # stochastic (a random generator per thread): it evaluates without errors, and x = 0 is reproducible
        def evaluate(_):
            problem = ioh.get_problem(2200, 1, 1, ioh.ProblemClass.GRAPH)
            n = problem.meta_data.n_variables
            values = [problem(np.random.default_rng(i).integers(0, 2, n)) for i in range(20)]
            return problem(np.zeros(n, dtype=int)), values

        results = in_threads(evaluate)
        self.assertEqual(len({zero for zero, _ in results}), 1)

    def test_loggers(self):
        # each thread logs its own problem; the loggers share the module's predefined triggers and properties
        triggers = [ioh.logger.trigger.ALWAYS, ioh.logger.trigger.ON_IMPROVEMENT]
        properties = [ioh.logger.property.RAWYBEST, ioh.logger.property.CURRENTY]
        inputs = points(CASES[1], 200)

        def log(_):
            store = ioh.logger.Store(triggers, properties)
            problem = ioh.get_problem(*CASES[1])
            problem.attach_logger(store)
            for x in inputs:
                problem(x)
            return store.data()

        expected = log(None)
        for result in in_threads(log):
            self.assertEqual(result, expected)

    def test_registration(self):
        # threads wrap problems at the same time: each gets its own id, and finds its own function
        def wrap(i):
            name = f"TestThreads{i}"
            ioh.wrap_problem(lambda x, i=i: float(i + sum(x)), name, ioh.ProblemClass.REAL, 2)
            problem = ioh.get_problem(name, 1, 2)
            return problem.meta_data.problem_id, problem([0.0, 0.0])

        results = in_threads(wrap)
        self.assertEqual(len({problem_id for problem_id, _ in results}), THREADS)
        self.assertEqual([y for _, y in results], [float(i) for i in range(THREADS)])

    def test_sobol_grids(self):
        # the Sobol sampler shares its state (one call at a time): problems that make their grids at the same time,
        # of different sizes and dimensions, get the grids that they get one at a time
        cases = [(40 + i, 1, 2 + i % 5, ioh.ProblemClass.REAL) for i in range(THREADS)]

        def make(i):
            problem = ioh.get_problem(*cases[i])
            return problem(np.full(problem.meta_data.n_variables, 0.5))

        expected = [make(i) for i in range(THREADS)]
        for _ in range(3):
            self.assertEqual(in_threads(make), expected)

    def test_logger_folders(self):
        # Analyzers created at the same time with the same root and folder name get a folder each
        with tempfile.TemporaryDirectory() as root:
            loggers = in_threads(lambda _: ioh.logger.Analyzer(root=root, folder_name="run"))
            # (closed afterwards: an Analyzer that logged nothing removes its folder when it is closed)
            self.assertEqual(len({logger.output_directory for logger in loggers}), THREADS)
            for logger in loggers:
                logger.close()

    def test_wrap_same_name(self):
        # threads that wrap functions under the same name each get a problem with their own function
        def wrap(i):
            problem = ioh.wrap_problem(lambda x, i=i: float(i), "TestThreadsSameName", ioh.ProblemClass.REAL, 2)
            return problem([0.0, 0.0])

        self.assertEqual(in_threads(wrap), [float(i) for i in range(THREADS)])

    def test_first_loads(self):
        # in a fresh interpreter, threads that create the same problems at once also load their data at once
        code = textwrap.dedent("""
            import threading
            from concurrent.futures import ThreadPoolExecutor
            import ioh

            cases = [(1001, 1, 10, ioh.ProblemClass.CEC2022), (1115, 1, 1, ioh.ProblemClass.CEC2013),
                     (2000, 1, 1, ioh.ProblemClass.GRAPH), (2300, 1, 1, ioh.ProblemClass.GRAPH)]
            barrier = threading.Barrier(8)

            def create(_):
                barrier.wait()
                return [ioh.get_problem(*case).meta_data.n_variables for case in cases]

            with ThreadPoolExecutor(8) as pool:
                results = list(pool.map(create, range(8)))
            assert all(r == results[0] for r in results), results
            """)
        env = dict(os.environ, PYTHONPATH=os.pathsep.join(sys.path))
        result = subprocess.run([sys.executable, "-c", code], capture_output=True, text=True, env=env)
        self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
