import os
import shutil
import unittest

class TestProblem(unittest.TestCase):

    def setUp(self) -> None:
        self.cwd = os.getcwd() 
        self.folder = "/tmp/IOHexperimenter"
        return super().setUp()

    def tearDown(self) -> None:
        shutil.rmtree(self.folder, True)
        os.chdir(self.cwd)
        return super().tearDown()

    def test_downloader(self):
        self.assertFalse(os.path.isdir(self.folder))
        os.chdir("/tmp")
        import ioh
        self.assertTrue(any(ioh.ProblemType.GRAPH.problems))
                

if __name__ == "__main__":
    unittest.main()