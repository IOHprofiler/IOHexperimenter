import os
import ast
import ioh
import unittest


DATA_DIR = os.path.join(
    os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "static"
)

class TestDynamicBinVal(unittest.TestCase):

    def test_pairs_from_static_file(self):
        with open(os.path.join(DATA_DIR, 'dynamic_bin_val.in'), 'r') as file:

            for line in file:
                data = line.strip().split()
                
                if not data: continue

                problem_id = int(data[0])            # e.g., 10001
                instance = int(data[1])              # e.g., 1
                num_timesteps = int(data[2])         # e.g., 34
                function_name = data[3]              # e.g., 'operator_call'

                if function_name == 'operator_call':
                    x = [int(char) for char in data[4]]  # e.g., 10
                    target_y = float(data[5])            # e.g., 0.69188

                    dynamic_bin_val_landscape = ioh.get_problem(problem_id, instance, len(x), ioh.ProblemClass.INTEGER)

                    for _ in range(num_timesteps):
                        dynamic_bin_val_landscape.step()
                    real_y = dynamic_bin_val_landscape(x)
                    self.assertAlmostEqual(real_y, target_y, places=4)

                if function_name == 'rank_indices':
                    input_bitstrings = ast.literal_eval(data[4]) # e.g., [[1,1]]
                    target_ranked_bitstrings = ast.literal_eval(data[5]) # [[1,1]]

                    dynamic_bin_val_landscape = ioh.get_problem(problem_id, instance, len(input_bitstrings[0]), ioh.ProblemClass.INTEGER)

                    for _ in range(num_timesteps):
                        dynamic_bin_val_landscape.step()

                    real_ranked_bitstrings = dynamic_bin_val_landscape.rank_indices(input_bitstrings)
                    self.assertEqual(target_ranked_bitstrings, real_ranked_bitstrings)

                if function_name == 'rank':
                    input_bitstrings = ast.literal_eval(data[4]) #  [[1,1]]
                    target_ranked_bitstrings = ast.literal_eval(data[5]) # [0]

                    dynamic_bin_val_landscape = ioh.get_problem(problem_id, instance, len(input_bitstrings[0]), ioh.ProblemClass.INTEGER)

                    for _ in range(num_timesteps):
                        dynamic_bin_val_landscape.step()

                    real_ranked_bitstrings = dynamic_bin_val_landscape.rank(input_bitstrings)
                    self.assertEqual(target_ranked_bitstrings, real_ranked_bitstrings)


if __name__ == '__main__':
  unittest.main()
