import unittest
import ioh

class AddTest(unittest.TestCase):
    def test_add(self):
        self.assertEqual(2, ioh.add(1, 1))


if __name__ == "__main__":
    unittest.main()