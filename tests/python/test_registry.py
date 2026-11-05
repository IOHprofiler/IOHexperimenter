import json
import os
import re
import subprocess
import sys
import unittest

import ioh
from ioh import suite_registry

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SUITE_HPP = os.path.join(ROOT, "include", "ioh", "suite.hpp")


def suite_headers():
    '''name -> (max_instance, max_dimension) of the suite classes in include/ioh/suite.hpp'''
    with open(SUITE_HPP, encoding="utf-8") as f:
        text = f.read()
    result = {}
    for match in re.finditer(r'struct (\w+) final : Suite<.*?registry_name = "(\w+)".*?(?=\n    (?://!|struct)|\Z)',
                             text, re.S):
        body = match.group(0)
        limits = re.search(r"registry_name,\s*(\d+),\s*(\d+),", body)
        result[match.group(2)] = tuple(int(g) for g in limits.groups()) if limits else None
    return result


class TestRegistry(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.registry = ioh.registry()
        cls.suites = cls.registry["suites"]

    def test_structure(self):
        self.assertEqual(self.registry["schema_version"], suite_registry.SCHEMA_VERSION)
        self.assertEqual(
            set(self.suites),
            {"BBOB", "SBOX", "PBO", "Submodular", "RealStarDiscrepancy", "IntegerStarDiscrepancy"},
        )
        for name, entry in self.suites.items():
            self.assertEqual(entry["name"], name)
            self.assertTrue(hasattr(ioh.suite, name), name)
            for key in ("instances", "dimensions", "standard_instances", "standard_dimensions", "budget",
                        "target_bounds", "scoring", "problems"):
                self.assertIn(key, entry)

    def test_problems_match_the_problem_classes(self):
        for name, entry in self.suites.items():
            problem_class = ioh.ProblemClass[entry["problem_class"]]
            listed = {p["id"]: p["name"] for p in entry["problems"]}
            self.assertEqual(listed, dict(problem_class.problems), name)
            self.assertEqual(entry["variable_type"], "real" if problem_class.is_real() else "integer")

    def test_problem_fields_match_the_problems(self):
        for name, entry in self.suites.items():
            problem_class = ioh.ProblemClass[entry["problem_class"]]
            for p in entry["problems"]:
                dimension = p["optimum"]["dimension"]
                problem = ioh.get_problem(p["id"], p["optimum"]["instance"], dimension, problem_class)
                self.assertEqual(problem.meta_data.name, p["name"])
                self.assertEqual(problem.meta_data.problem_id, p["id"])
                self.assertEqual(
                    problem.meta_data.optimization_type == ioh.OptimizationType.MAX, p["maximization"])
                self.assertEqual(p["optimization_type"], "MAX" if p["maximization"] else "MIN")
                self.assertEqual(float(problem.bounds.lb[0]), p["bounds"]["lower"])
                self.assertEqual(float(problem.bounds.ub[0]), p["bounds"]["upper"])
                self.assertEqual(p["known_optimum"], p["optimum"]["value"] is not None)
                if p["known_optimum"]:
                    self.assertEqual(float(problem.optimum.y), p["optimum"]["value"])
                if p["fixed_dimension"] is not None:
                    self.assertEqual(problem.meta_data.n_variables, p["fixed_dimension"])

    def test_known_values(self):
        bbob = {p["name"]: p for p in self.suites["BBOB"]["problems"]}
        self.assertEqual(len(bbob), 24)
        self.assertFalse(bbob["Sphere"]["maximization"])
        self.assertTrue(bbob["Sphere"]["optimum"]["depends_on_instance"])
        self.assertFalse(bbob["Sphere"]["optimum"]["depends_on_dimension"])
        pbo = {p["name"]: p for p in self.suites["PBO"]["problems"]}
        self.assertTrue(pbo["OneMax"]["maximization"])
        self.assertEqual(pbo["OneMax"]["optimum"]["value"], 16.0)
        self.assertEqual(pbo["IsingTriangular"]["dimension_requirement"], "perfect_square")
        graph = self.suites["Submodular"]["problems"][0]
        self.assertEqual(graph["fixed_dimension"], 800)
        self.assertFalse(graph["known_optimum"])

    def test_conventions_are_usable(self):
        for name, entry in self.suites.items():
            for key, standard in (("instances", "standard_instances"), ("dimensions", "standard_dimensions")):
                limits = entry[key]
                self.assertLessEqual(limits["min"], limits["max"])
                self.assertTrue(entry[standard], name)
                for value in entry[standard]:
                    self.assertTrue(limits["min"] <= value <= limits["max"], (name, standard, value))
            budget = entry["budget"]
            if budget is not None:
                self.assertGreater(budget["base"] + budget["per_dimension"], 0)
            bounds = entry["target_bounds"]
            self.assertIn(bounds["scale"], ("log", "linear"))
            if bounds["lower"] is not None and bounds["upper"] is not None:
                self.assertLess(bounds["lower"], bounds["upper"])
            # the standard instances and dimensions create the suite's problems
            first = entry["problems"][0]["id"]
            s = getattr(ioh.suite, name)([first], entry["standard_instances"][:1], entry["standard_dimensions"][:1])
            self.assertEqual(len(list(s)), 1)

    def test_limits_match_the_cpp_suites(self):
        if not os.path.exists(SUITE_HPP):
            self.skipTest("include/ioh/suite.hpp is not available")
        limits = suite_headers()
        for name in ("Real", "Integer"):
            limits.pop(name)
        self.assertEqual(set(limits), set(self.suites))
        for name, entry in self.suites.items():
            max_instance, max_dimension = limits[name]
            self.assertEqual(entry["instances"]["max"], max_instance, name)
            if name != "Submodular":  # ignores its dimensions: its limit is a fixed dimension per problem
                self.assertEqual(entry["dimensions"]["max"], max_dimension, name)

    def test_default_dimensions_are_in_range(self):
        for name, entry in self.suites.items():
            s = getattr(ioh.suite, name)([entry["problems"][0]["id"]])
            for instance in s.instances:
                self.assertTrue(entry["instances"]["min"] <= instance <= entry["instances"]["max"])
            for dimension in s.dimensions:
                self.assertTrue(entry["dimensions"]["min"] <= dimension <= entry["dimensions"]["max"])

    def test_scoring_is_aocc_by_default(self):
        for name, entry in self.suites.items():
            self.assertEqual(entry["scoring"]["method"], "AOCC")
            self.assertEqual(ioh.suite_registry.scoring(name), entry["scoring"])
            for p in entry["problems"]:
                self.assertIsNone(p["scoring"])
        self.assertEqual(suite_registry.scoring("BBOB", 3)["method"], "AOCC")
        with self.assertRaises(KeyError):
            suite_registry.scoring("BBOB", 99)

    def test_scoring_overrides(self):
        override = {"method": "ECDF", "parameters": {"targets": 51}}
        try:
            suite_registry.SCORING_OVERRIDES["BBOB"] = {2: override}
            suite_registry._cache = None
            self.assertEqual(suite_registry.scoring("BBOB", 2), override)
            self.assertEqual(suite_registry.scoring("BBOB", 1)["method"], "AOCC")
            self.assertEqual(suite_registry.scoring("BBOB")["method"], "AOCC")
            self.assertEqual(suite_registry.scoring("PBO", 2)["method"], "AOCC")
        finally:
            suite_registry.SCORING_OVERRIDES.clear()
            suite_registry._cache = None

    def test_registry_is_a_copy(self):
        ioh.registry()["suites"]["BBOB"]["problems"].clear()
        self.assertEqual(len(ioh.registry()["suites"]["BBOB"]["problems"]), 24)

    def test_json(self):
        self.assertEqual(json.loads(suite_registry.to_json()), self.registry)
        process = subprocess.run([sys.executable, "-m", "ioh.suite_registry"], capture_output=True, text=True)
        self.assertEqual(process.returncode, 0, process.stderr)
        self.assertEqual(json.loads(process.stdout), self.registry)


if __name__ == "__main__":
    unittest.main()
