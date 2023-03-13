import os
import shutil
import sys
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

    @unittest.skipUnless(sys.platform != "win32", "windows tmp dir")
    def test_downloader(self):
        self.assertFalse(os.path.isdir(self.folder))
        os.chdir("/tmp")
        import ioh
        self.assertTrue(any(ioh.ProblemClass.GRAPH.problems))


if __name__ == "__main__":
    unittest.main()