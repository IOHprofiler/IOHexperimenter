"""Golden tests: the Python-visible registry and problem behaviour must stay identical to master.

See tests/python/golden.py and TASKS.md (T1.1). Never regenerate the fixtures to make these pass.
"""
import unittest

import ioh

try:
    from . import golden
except ImportError:  # executed as a script, e.g. by cibuildwheel's test-command
    import golden


class TestGoldenRegistry(unittest.TestCase):
    def test_registry_matches_fixture(self):
        expected = golden.load(golden.REGISTRY_FILE)
        actual = golden.fresh_registry_snapshot()
        for section in ("problem_classes", "views", "wrap"):
            with self.subTest(section=section):
                golden.assert_close(self, expected[section], actual[section], section)


class TestGoldenBehaviour(unittest.TestCase):
    def test_behaviour_matches_fixture(self):
        expected = golden.load(golden.BEHAVIOUR_FILE)
        actual = golden.behaviour_records(ioh, golden.load(golden.REGISTRY_FILE))
        self.assertEqual(sorted(expected), sorted(actual), "set of records differs")
        for key in sorted(expected):
            with self.subTest(record=key):
                golden.assert_record(self, expected[key], actual[key], key)

    def test_union_classes_match_families(self):
        registry = golden.load(golden.REGISTRY_FILE)
        classes = golden.problem_classes(ioh)
        for union, pid, family in golden.union_pairs(registry):
            with self.subTest(union=union, id=pid):
                self.assertIsNotNone(family, f"{union} id {pid} belongs to no family")
                dims, instances, _ = golden.FAMILIES.get(family, golden.DYNAMIC_BIN_VAL)
                key = f"union/{union}/{pid}"
                # check registration first: get_problem() downloads data when GRAPH is empty, and
                # Factory::create only asserts that the id exists (undefined behaviour in Release)
                self.assertIn(pid, golden.live_registry(ioh, classes[union]))
                if family == "DynamicBinVal":
                    self.assertIn(pid, ioh.iohcpp.problem.DynamicBinVal.problems)
                    reference = ioh.iohcpp.problem.DynamicBinVal.create(pid, instances[0], dims[0])
                else:
                    self.assertIn(pid, golden.live_registry(ioh, classes[family]))
                    reference = ioh.get_problem(pid, instances[0], dims[0], classes[family])
                via_union = ioh.get_problem(pid, instances[0], dims[0], classes[union])
                golden.assert_record(self, golden._describe(reference, ioh, key, 1),
                                     golden._describe(via_union, ioh, key, 1), key)


if __name__ == "__main__":
    unittest.main()
