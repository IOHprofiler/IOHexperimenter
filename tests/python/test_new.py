import unittest
import ioh

class Add2Test(unittest.TestCase):
    def test_add(self):
        self.assertEqual(3, ioh.add(1, 2))

    def test_to_lower(self):
        self.assertEqual("hallo", ioh.to_lower("HALLO"))


if __name__ == "__main__":
    unittest.main()