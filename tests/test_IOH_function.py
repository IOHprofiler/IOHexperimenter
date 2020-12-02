import unittest
import numpy as np

from IOHexperimenter import IOH_function, custom_IOH_function, W_model_function


class PBOTestCase(unittest.TestCase):
    def test_initialization_PBO(self):
        f = IOH_function(1, 16, 1, suite = "PBO")
        assert isinstance(f, IOH_function)
        assert f.maximization
        assert f.number_of_variables == 16
        assert f.suite == "PBO"
        assert not f.final_target_hit
        assert all(f.upperbound == 1)
        assert all(f.lowerbound == 0)
        
    def test_initialization_BBOB(self):
        f = IOH_function(1, 5, 1, suite = "BBOB")
        assert isinstance(f, IOH_function)
        assert not f.maximization
        assert f.number_of_variables == 5
        assert f.suite == "BBOB"
        assert not f.final_target_hit
        assert all(f.upperbound == 5)
        assert all(f.lowerbound == -5)
        
    def test_initialization_custom(self):
        def internal_func(x):
            return np.sum(x)
        f = custom_IOH_function(internal_func, "sum", 5)
        assert isinstance(f, IOH_function)
        assert not f.maximization
        assert f.number_of_variables == 5
        assert f.suite == "No Suite"
        assert not f.final_target_hit
        assert all(f.upperbound == 5)
        assert all(f.lowerbound == -5)

    def test_W_model(self):
        f = W_model_function()
        assert isinstance(f, IOH_function)
        assert f.maximization
        assert f.number_of_variables == 16
        assert f.suite == "W_model"
        assert not f.final_target_hit
        f2 = IOH_function(1, 16, 0, suite = "PBO")
        assert f(16 * [0]) == f2(16 * [0])
        assert f(16 * [1]) == f2(16 * [1])

    def test_function_calling(self):
        f = IOH_function(1, 5, 1, suite = "BBOB")
        assert f([0] * 5) == f(np.array([0] * 5))
        assert all(f.best_so_far_variables == np.array([0] * 5))
        assert f.evaluations == 2
        assert f.best_so_far_fvalue == f(np.array([0] * 5))
        f.reset()
        assert f.evaluations == 0
        assert f.best_so_far_variables is None

    def test_random_search(self):
        def random_search(func, budget=None):
            if budget is None:
                budget = int(func.number_of_variables * 1e4)

            f_opt = np.Inf
            x_opt = None

            for _ in range(budget):
                x = np.random.uniform(func.lowerbound, func.upperbound)
                f = func(x)
                if f < f_opt:
                    f_opt = f
                    x_opt = x

            return f_opt, x_opt


        f = IOH_function(2, 5, 1)
        random_search(f)


if __name__ == '__main__':
    unittest.main()