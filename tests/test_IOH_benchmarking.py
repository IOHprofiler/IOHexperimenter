from .utils import opt_alg, TmpdirTestCase
from IOHexperimenter import IOHexperimenter


class BenchmarkTestCase(TmpdirTestCase):    
    def test_initialization_PBO(self):
        exp = IOHexperimenter()
        exp.initialize_BBOB([1,2], [5], [5], 2, 1e-8)
        exp.set_logger_location(self.tmpdir, "run")
        a = opt_alg(1000)
        exp([a])
    
if __name__ == '__main__':
    unittest.main()