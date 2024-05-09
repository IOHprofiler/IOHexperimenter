import unittest
from unittest.mock import patch
import ioh

class TestDynamicBinVal(unittest.TestCase):

    @patch('ioh.get_problem')
    def test_dynamic_bin_val_uniform(self, mock_get_problem):
        mock_get_problem.return_value = mock_problem = MagicMock()

        f1 = mock_problem
        x1 = [0, 1, 1, 1, 0]
        expected_calls = [([0, 1, 1, 1, 0],), ([0, 1, 1, 1, 0],), ([0, 1, 1, 1, 0],), ([0, 1, 1, 1, 0],), ([0, 1, 1, 1, 0],)]

        # Test instance 1
        with patch('builtins.print') as mock_print:
            self.assertEqual(f1(x1), mock_print.call_args[0][0])
            f1.step()
            self.assertEqual(f1(x1), mock_print.call_args[0][0])
            f1.step()
            self.assertEqual(f1(x1), mock_print.call_args[0][0])
            self.assertEqual(f1(x1), mock_print.call_args[0][0])
            f1.step()
            self.assertEqual(f1(x1), mock_print.call_args[0][0])

        mock_get_problem.assert_called_with(10001, 1, 5, ioh.ProblemClass.INTEGER)
        mock_problem.assert_called_with(*expected_calls)

if __name__ == '__main__':
    unittest.main()
