"""Test behaviour of the bindings that the port to nanobind (WP6) had to keep, or that its review found untested."""

import gc
import os
import tempfile
import unittest

import numpy as np

import ioh


class TestBindings(unittest.TestCase):
    def test_loggers_on_suites(self):
        # a suite tells its loggers its name (attach_suite, which has no binding)
        with tempfile.TemporaryDirectory() as root:
            store = ioh.logger.Store([ioh.logger.trigger.ALWAYS], [])
            # Combine holds its loggers by reference, so they must stay alive (as before)
            combined = ioh.logger.Store([ioh.logger.trigger.ALWAYS], [])
            loggers = [
                store,
                ioh.logger.FlatFile([ioh.logger.trigger.ALWAYS], [], output_directory=root),
                ioh.logger.Analyzer(root=root, folder_name="run"),
                ioh.logger.Combine([combined]),
            ]
            for logger in loggers:
                suite = ioh.suite.BBOB([1, 2], [1], [2])
                suite.attach_logger(logger)
                for problem in suite:
                    problem([0.0, 0.0])
            self.assertEqual(list(store.data().keys()), ["BBOB"])
            self.assertEqual(list(combined.data().keys()), ["BBOB"])
            # release the files before the directory is removed (Windows does not remove an open file): the
            # Analyzer closes them with close(), the FlatFile when it is destroyed
            loggers[2].close()
            del loggers, suite, problem
            gc.collect()

    def test_logger_overrides(self):
        # an override of a Python subclass runs once: when C++ calls it, and when it calls the base with super()
        calls = []

        class MyStore(ioh.logger.Store):
            def attach_problem(self, meta_data):
                calls.append("attach_problem")
                super().attach_problem(meta_data)

            def call(self, info):  # pylint: disable=arguments-differ  (a compiled method, see Store.call)
                calls.append("call")
                super().call(info)

        store = MyStore([ioh.logger.trigger.ALWAYS], [])
        problem = ioh.get_problem(1, 1, 2)
        problem.attach_logger(store)
        problem([0.0, 0.0])
        self.assertEqual(calls, ["attach_problem", "call"])
        calls.clear()
        store.attach_problem(problem.meta_data)
        store.call(problem.log_info)
        self.assertEqual(calls, ["attach_problem", "call"])
        self.assertTrue(store.data())

        class Recorder(ioh.logger.AbstractLogger):
            def __init__(self):
                super().__init__([ioh.logger.trigger.ALWAYS])
                self.events = []

            def attach_suite(self, name):
                self.events.append(("suite", name))

            def attach_problem(self, meta_data):
                super().attach_problem(meta_data)
                self.events.append(("problem", meta_data.problem_id))

            def __call__(self, info):
                self.events.append(("call",))

        recorder = Recorder()
        suite = ioh.suite.BBOB([1], [1], [2])
        suite.attach_logger(recorder)
        for problem in suite:
            problem([0.0, 0.0])
        self.assertEqual(recorder.events[0], ("suite", "BBOB"))
        self.assertIn(("problem", 1), recorder.events)
        self.assertIn(("call",), recorder.events)

    def test_default_analyzer_triggers(self):
        # Analyzers made with the default triggers each keep the best value of their own run (they shared one trigger)
        with tempfile.TemporaryDirectory() as root:
            loggers = [ioh.logger.Analyzer(root=root, folder_name=name) for name in ("a", "b")]
            problems = [ioh.get_problem(1, 1, 2) for _ in loggers]
            for problem, logger in zip(problems, loggers):
                problem.attach_logger(logger)
            problems[0]([0.0, 0.0])
            problems[1]([4.0, 4.0])  # worse than the other run's best, but the first evaluation of its own run
            for logger in loggers:
                logger.close()
            rows = []
            for path, _, files in os.walk(os.path.join(root, "b")):
                for name in files:
                    if name.endswith(".dat"):
                        with open(os.path.join(path, name)) as f:
                            rows += [line.split()[0] for line in f if line.strip()]
            del loggers, problems
            gc.collect()
            self.assertIn("1", rows)

    def test_analyzer_folders(self):
        # an Analyzer makes a folder of its own, with the folders above a name with separators (e.g. Experiment's)
        with tempfile.TemporaryDirectory() as root:
            loggers = [ioh.logger.Analyzer(root=root, folder_name=name) for name in ("a/b", "a/b", "c/")]
            folders = [os.path.relpath(logger.output_directory, root) for logger in loggers]
            self.assertEqual(folders, [os.path.normpath(f) for f in ("a/b", "a/b-1", "c")])
            for logger in loggers:
                logger.close()

    def test_integer_arrays(self):
        # integer problems take arrays of any integer or bool type (e.g. a bitstring as rng.random(n) < 0.5), but not
        # of floats, and not values out of the range of int; so do the setters of solutions and bounds (WP7)
        problem = ioh.get_problem(1, 1, 6, ioh.ProblemClass.PBO)
        bits = np.array([1, 0, 1, 1, 0, 1])
        expected = problem(bits.tolist())
        for dtype in (bool, np.int8, np.uint8, np.int16, np.int32, np.int64, np.uint64):
            self.assertEqual(problem(bits.astype(dtype)), expected, dtype)
        y = problem(np.vstack([bits, bits]).astype(bool))
        self.assertEqual(y.dtype, np.float64)
        self.assertEqual(y.tolist(), [expected, expected])
        solution = ioh.IntegerSolution(bits.astype(bool), 1.0)
        self.assertEqual(list(solution.x), bits.tolist())
        bounds = ioh.IntegerBounds(np.zeros(6, dtype=bool), np.ones(6, dtype=bool))
        self.assertEqual(list(bounds.ub), [1] * 6)
        for dtype in (bool, np.uint8, np.int64):
            solution.x = (1 - bits).astype(dtype)
            bounds.lb = (1 - bits).astype(dtype)
            self.assertEqual((solution.x.tolist(), bounds.lb.tolist()), ((1 - bits).tolist(),) * 2, dtype)
        for bad in (bits.astype(float), np.array([2**40, 0, 0, 0, 0, 0])):
            with self.assertRaises(TypeError):
                problem(bad)
            with self.assertRaises(TypeError):
                solution.x = bad
            with self.assertRaises(TypeError):
                bounds.ub = bad

    def test_callback_results(self):
        # a Python function that returns something unusable raises RuntimeError naming what it returned
        ioh.wrap_problem(lambda x: None, "TestBindingsReturnsNone", ioh.ProblemClass.REAL, 2)
        problem = ioh.get_problem("TestBindingsReturnsNone", 1, 2)
        with self.assertRaisesRegex(RuntimeError, "NoneType"):
            problem([0.0, 0.0])

    def test_suite_docstrings(self):
        # a suite's docstring lists its arguments problem_ids, instances and dimensions (it listed problem_ids twice)
        for suite in (ioh.suite.BBOB, ioh.suite.PBO):
            for argument in ("problem_ids", "instances", "dimensions"):
                self.assertEqual(suite.__doc__.count(f"{argument}: list[int]"), 1, argument)


if __name__ == "__main__":
    unittest.main()
