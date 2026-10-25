import os
import subprocess
import sys
import tempfile
import textwrap
import unittest

import ioh


def run(code, **env):
    """Runs code in a fresh interpreter, from an empty working directory"""
    with tempfile.TemporaryDirectory() as cwd:
        environment = dict(os.environ, PYTHONPATH=os.pathsep.join(os.path.abspath(p) for p in sys.path), **env)
        if "IOH_RESOURCES" not in env:
            environment.pop("IOH_RESOURCES", None)
        return subprocess.run([sys.executable, "-c", textwrap.dedent(code)], capture_output=True, text=True,
                              cwd=cwd, env=environment)


class TestData(unittest.TestCase):
    def test_package_data(self):
        # the data files come with the package: no download, no IOH_RESOURCES, any working directory
        result = run("""
            import os, ioh
            assert "IOH_RESOURCES" not in os.environ
            assert os.path.samefile(ioh.data_root(), os.path.join(os.path.dirname(ioh.__file__), "static"))
            assert len(ioh.ProblemClass.GRAPH.problems) == 78
            assert ioh.get_problem(2000, 1, 1, ioh.ProblemClass.GRAPH).meta_data.n_variables == 800
            """)
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_resources_must_exist(self):
        result = run("import ioh", IOH_RESOURCES=os.path.join(tempfile.gettempdir(), "ioh-no-such-directory"))
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("ImportError", result.stderr)

    def test_set_data_root(self):
        # another data directory, then back to the package's
        result = run("""
            import os, tempfile, ioh
            package = ioh.data_root()
            with tempfile.TemporaryDirectory() as empty:
                ioh.set_data_root(empty)
                assert os.path.samefile(ioh.data_root(), empty)
                try:
                    ioh.get_problem(1001, 1, 10, ioh.ProblemClass.CEC2022)
                    raise AssertionError("no error without data")
                except ioh.DataError:
                    pass
            ioh.set_data_root(package)
            assert ioh.get_problem(1001, 1, 10, ioh.ProblemClass.CEC2022).meta_data.n_variables == 10
            """)
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_deprecated_helpers(self):
        with self.assertWarns(DeprecationWarning):
            ioh.download_static_folder()
        with self.assertWarns(DeprecationWarning):
            ioh.load_graph_problems()


if __name__ == "__main__":
    unittest.main()
