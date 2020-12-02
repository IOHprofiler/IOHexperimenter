import os

from .utils import opt_alg, TmpdirTestCase
from IOHexperimenter import IOH_function, IOH_logger


class LoggerTestCase(TmpdirTestCase):    
    def test_initialization_logger(self):
        l = IOH_logger(self.tmpdir, "temp_run")
        l.clear_logger()
        assert 1
        
    def test_logging_fct(self):
        f = IOH_function(1, 5, 1, suite = "BBOB")
        l = IOH_logger(self.tmpdir, "temp_run")
        f.add_logger(l)
        f([0] * 5)
        assert os.path.exists(f"{self.tmpdir}/temp_run")
        f.clear_logger()
        
    def test_logging_with_params(self):
        f = IOH_function(1, 5, 1, suite = "BBOB")
        l = IOH_logger(self.tmpdir, "temp_run")
        a = opt_alg(10000)
        l.track_parameters(a, 'param_rate')
        l.set_static_attributes('static_param')
        l.set_dynamic_attributes('dynamic_prop')
        f.add_logger(l)
        a(f)
        assert os.path.exists(f"{self.tmpdir}/temp_run")
        f.clear_logger()

if __name__ == '__main__':
    unittest.main()