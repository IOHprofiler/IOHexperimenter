import os
import unittest
import subprocess


BASE_DIR = os.path.realpath(
    os.path.join(os.path.dirname(__file__), "../../")
)

class MetaTest(type):
    def __new__(cls, name, bases, dct):
        instance = super().__new__(cls, name, bases, dct)
        dirname = os.path.normpath(os.path.join(BASE_DIR, "example"))
        for f in filter(lambda x: x.endswith("ipynb"), os.listdir(dirname)):
            fname, *_ = os.path.basename(f).split(".")
            notebook =  os.path.join(dirname, f)
            def test_notebook_runner(self):
                self.assertTrue(os.path.isfile(notebook))
                result = subprocess.run([
                    "jupyter", "nbconvert", "--to", "python",  "--execute", notebook],
                    stdout=subprocess.PIPE, stderr=subprocess.PIPE
                )
                self.assertEqual(result.returncode, 0, result.stderr.decode("utf-8"))
                os.remove(os.path.join(dirname, f"{fname}.py"))
            setattr(instance, f"test_notebook_{fname}", test_notebook_runner)

        return instance

class TestExamples(unittest.TestCase, metaclass=MetaTest):
    pass


if __name__ == "__main__":
    unittest.main()