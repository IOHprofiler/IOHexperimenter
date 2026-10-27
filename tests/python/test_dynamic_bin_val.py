import ast
import os
import unittest

import numpy as np

import ioh


# the data files of the tests, in tests/data
DATA_DIR = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "data")

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
                    self.assertEqual(target_ranked_bitstrings, real_ranked_bitstrings.tolist())

                if function_name == 'rank':
                    input_bitstrings = ast.literal_eval(data[4]) #  [[1,1]]
                    target_ranked_bitstrings = ast.literal_eval(data[5]) # [0]

                    dynamic_bin_val_landscape = ioh.get_problem(problem_id, instance, len(input_bitstrings[0]), ioh.ProblemClass.INTEGER)

                    for _ in range(num_timesteps):
                        dynamic_bin_val_landscape.step()

                    real_ranked_bitstrings = dynamic_bin_val_landscape.rank(input_bitstrings)
                    self.assertEqual(target_ranked_bitstrings, real_ranked_bitstrings.tolist())

    def test_arrays(self):
        # rank, rank_indices, get_comparison_ordering and weights return new arrays, for lists and arrays alike
        ranking = ioh.problem.DynamicBinValRanking(1, 4)
        bitstrings = [[1, 0, 1, 0], [0, 0, 0, 0], [1, 1, 1, 1]]
        for value in (bitstrings, np.array(bitstrings, dtype=bool)):
            ranked = ranking.rank(value)
            self.assertIsInstance(ranked, np.ndarray)
            self.assertEqual((ranked.dtype, ranked.shape, ranked.flags.writeable), (np.int32, (3, 4), True))
            indices = ranking.rank_indices(value)
            self.assertIsInstance(indices, np.ndarray)
            self.assertEqual((indices.dtype, indices.shape, indices.flags.writeable), (np.int32, (3,), True))
            self.assertEqual(ranked.tolist(), [bitstrings[i] for i in indices])

        # copies: a step changes the problem's ordering and weights, not the arrays read before
        ordering = ranking.get_comparison_ordering()
        self.assertIsInstance(ordering, np.ndarray)
        self.assertEqual((ordering.dtype, sorted(ordering)), (np.int32, [0, 1, 2, 3]))
        kept = ordering.tolist()
        for _ in range(5):
            ranking.step()
        self.assertNotEqual(ranking.get_comparison_ordering().tolist(), kept)
        self.assertEqual(ordering.tolist(), kept)
        for problem in (ranking, ioh.problem.DynamicBinValUniform(1, 4), ioh.problem.DynamicBinValPareto(1, 4),
                        ioh.problem.DynamicBinValPowersOfTwo(1, 4)):
            weights = problem.weights
            self.assertIsInstance(weights, np.ndarray)
            self.assertEqual((weights.dtype, weights.shape, weights.flags.writeable), (np.float64, (4,), True))
            if problem is not ranking:  # whose weights are not used (zeros)
                kept = weights.tolist()
                for _ in range(5):
                    problem.step()
                self.assertNotEqual(problem.weights.tolist(), kept)
                self.assertEqual(weights.tolist(), kept)


if __name__ == '__main__':
  unittest.main()
