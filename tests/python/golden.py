"""Golden fixtures that freeze the Python-visible behaviour of ioh (see TASKS.md, task T1.1).

The fixtures in tests/data/golden were generated from an unchanged build of master and are the
reference for every refactor of the registry, the data handling and the bindings. They cover every
registry (ids and names), the behaviour of every registered problem (metadata, bounds, optimum and
objective values at deterministic points), suite iteration, and directly constructed problems.
Regenerating them requires explicit maintainer approval and a justification in the pull request.

Usage (run against an installed / built ioh):
    python tests/python/golden.py --dump-registry   # print the registry snapshot of a fresh interpreter
    python tests/python/golden.py --write           # regenerate tests/data/golden (needs approval!)
"""
import json
import math
import os
import random
import sys
import zlib

FIXTURE_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "data", "golden")
REGISTRY_FILE = os.path.join(FIXTURE_DIR, "registry.json")
BEHAVIOUR_FILE = os.path.join(FIXTURE_DIR, "behaviour.json")

REL_TOL = 1e-9
ABS_TOL = 1e-12
FULL_VECTOR_MAX = 100  # longer vectors are stored as {"len": n, "sum": s}

# ProblemClass name -> (dimensions, instances, number of evaluated points)
FAMILIES = {
    "BBOB": ([2, 10], [1, 2], 3),
    "SBOX": ([2, 10], [1, 2], 3),
    "PBO": ([16, 64], [1, 2], 3),  # ids 21 and 23 require perfect squares
    "CEC2013": ([2], [1, 2], 3),  # the dimension is fixed per id; the requested one is ignored
    "CEC2022": ([10, 20], [1, 2], 3),  # transformation data exists for 10 and 20 for every function
    "STAR_REAL": ([2, 5], [1, 2], 3),
    "STAR_INTEGER": ([2, 5], [1, 2], 3),
    "GRAPH": ([2], [1], 2),  # dimension and instance are ignored (fixed graph instances)
}
DYNAMIC_BIN_VAL = ([10], [1, 2], 3)
WRAP_NAMES = ("golden_wrap_real", "golden_wrap_integer")

# MaxInfluence evaluates Monte-Carlo cascades with the global, randomly seeded generator
# (max_influence.hpp); only x = 0 (no seed nodes) has a reproducible objective value
STOCHASTIC_PREFIXES = ("MaxInfluence",)

# suite name -> (problem_ids, instances, dimensions); iteration order and contents are frozen
SUITES = {
    "Real": ([1, 24, 30, 1001, 1101], [1], [10]),
    "BBOB": ([1, 12, 24], [1, 2], [2, 5]),
    "SBOX": ([1, 12, 24], [1, 2], [2, 5]),
    "RealStarDiscrepancy": ([30, 45, 59], [1, 2], [2]),
    "Integer": ([1, 21, 30, 2000, 10001], [1], [16]),
    "PBO": ([1, 20, 25], [1, 2], [16]),
    "Submodular": ([2000, 2100, 2200, 2300], [1], [1]),
    "IntegerStarDiscrepancy": ([30, 59], [1], [3]),
}


def _seed(key):
    return zlib.crc32(key.encode())


def _vector(values):
    values = [float(v) for v in values]
    if len(values) > FULL_VECTOR_MAX:
        return {"len": len(values), "sum": math.fsum(values)}
    return values


def _points(key, lb, ub, count, integer):
    """Deterministic points within the bounds; random.Random.random() is stable across versions."""
    rng = random.Random(_seed(key))
    points = []
    for _ in range(count):
        if integer:
            x = [int(min(u, l + math.floor(rng.random() * (u - l + 1)))) for l, u in zip(lb, ub)]
        else:
            x = [l + (u - l) * rng.random() for l, u in zip(lb, ub)]
        points.append(x)
    return points


def _is_integer_problem(problem, ioh):
    return isinstance(problem, ioh.iohcpp.problem.IntegerSingleObjective)


def _is_stochastic(name):
    return str(name).startswith(STOCHASTIC_PREFIXES)


def _describe(problem, ioh, key, count, dynamic=False):
    meta = problem.meta_data
    if _is_stochastic(meta.name):
        return _describe_stochastic(problem)
    lb, ub = list(problem.bounds.lb), list(problem.bounds.ub)
    integer = _is_integer_problem(problem, ioh)
    record = {
        "meta": {
            "problem_id": int(meta.problem_id),
            "name": str(meta.name),
            "instance": int(meta.instance),
            "n_variables": int(meta.n_variables),
            "optimization_type": str(meta.optimization_type.name),
        },
        "lb": _vector(lb),
        "ub": _vector(ub),
        "optimum_x": _vector(problem.optimum.x),
        "optimum_y": float(problem.optimum.y),
    }
    points = _points(key, lb, ub, count, integer)
    record["x_sums"] = [math.fsum(map(float, x)) for x in points]
    y = []
    for x in points:
        y.append(float(problem(x)))
        if dynamic:
            y.append(float(problem.step()))
            y.append(float(problem(x)))
    record["y"] = y
    return record


def _describe_stochastic(problem):
    """Like _describe, but only reproducible objective values (see STOCHASTIC_PREFIXES)."""
    meta = problem.meta_data
    n = int(meta.n_variables)
    single = [1] + [0] * (n - 1)
    return {
        "meta": {
            "problem_id": int(meta.problem_id),
            "name": str(meta.name),
            "instance": int(meta.instance),
            "n_variables": n,
            "optimization_type": str(meta.optimization_type.name),
        },
        "lb": _vector(problem.bounds.lb),
        "ub": _vector(problem.bounds.ub),
        "optimum_x": _vector(problem.optimum.x),
        "optimum_y": float(problem.optimum.y),
        "y_at_zero": float(problem([0] * n)),
        "single_node_is_finite": math.isfinite(float(problem(single))),
    }


def _safe(fn):
    try:
        return fn()
    except Exception as exc:  # behaviour on errors is part of the contract, but not the message
        return {"error": type(exc).__name__}


def problem_classes(ioh):
    return {pc.name: pc for pc in ioh.ProblemClass}


def live_registry(ioh, pc):
    """The registry behind a ProblemClass, read from the bound class.

    ProblemClass.problems and get_problem() download data into the working directory when the GRAPH
    registry is empty; reading the bound class directly never has side effects.
    """
    return getattr(ioh.iohcpp.problem, pc.value).problems


def registry_snapshot(ioh):
    """Every Python-visible problem registry: per ProblemClass and per bound class with `problems`."""
    snapshot = {"problem_classes": {}, "views": {}}
    for name, pc in problem_classes(ioh).items():
        snapshot["problem_classes"][name] = {str(k): v for k, v in sorted(live_registry(ioh, pc).items())}
    module = ioh.iohcpp.problem
    for name in sorted(dir(module)):
        try:
            problems = getattr(getattr(module, name), "problems")
        except Exception:
            continue
        if isinstance(problems, dict):
            snapshot["views"][name] = {str(k): v for k, v in sorted(problems.items())}
    return snapshot


def wrap_scenario(ioh):
    """Ids assigned to runtime-registered (wrapped) problems in a fresh interpreter."""
    result = {}
    for name, pc in zip(WRAP_NAMES, (ioh.ProblemClass.REAL, ioh.ProblemClass.INTEGER)):
        first = ioh.wrap_problem(lambda x: float(sum(x)), name, pc, dimension=3)
        again = ioh.wrap_problem(lambda x: float(sum(x)), name, pc, dimension=3)
        by_name = ioh.get_problem(name, 1, 3, pc)
        result[name] = {
            "id": int(first.meta_data.problem_id),
            "id_after_rewrap": int(again.meta_data.problem_id),
            "id_by_name": int(by_name.meta_data.problem_id),
            "y": float(by_name([1, 2, 3])),
        }
    return result


def behaviour_records(ioh, registry):
    """Records for every registered id of every family, plus classes that are not registered."""
    classes = problem_classes(ioh)
    records = {}
    for family, (dims, instances, count) in FAMILIES.items():
        pc = classes[family]
        live = live_registry(ioh, pc)
        for pid in registry["problem_classes"][family]:
            for iid in instances:
                for dim in dims:
                    key = f"{family}/{pid}/{iid}/{dim}"
                    if int(pid) not in live:  # never let get_problem() fall back to downloading
                        records[key] = {"error": "not registered"}
                        continue
                    records[key] = _safe(
                        lambda: _describe(ioh.get_problem(int(pid), iid, dim, pc), ioh, key, count))

    dbv = ioh.iohcpp.problem.DynamicBinVal
    dims, instances, count = DYNAMIC_BIN_VAL
    for pid in sorted(dbv.problems):
        for iid in instances:
            for dim in dims:
                key = f"DynamicBinVal/{pid}/{iid}/{dim}"
                records[key] = _safe(lambda: _describe(dbv.create(pid, iid, dim), ioh, key, count, dynamic=True))

    P = ioh.iohcpp.problem
    direct = {
        "WModelOneMax/default": lambda: P.WModelOneMax(1, 16),
        "WModelOneMax/configured": lambda: P.WModelOneMax(2, 16, 0.5, 2, 2, 3),
        "WModelLeadingOnes/default": lambda: P.WModelLeadingOnes(1, 16),
        "WModelLeadingOnes/configured": lambda: P.WModelLeadingOnes(2, 16, 0.5, 2, 2, 3),
        "DoubleSphere": lambda: P.DoubleSphere(5),
        "DoubleRastrigin": lambda: P.DoubleRastrigin(5),
        "ManyAffine/1": lambda: P.ManyAffine(1, 5),
        "ManyAffine/2": lambda: P.ManyAffine(2, 5),
    }
    for key, make in direct.items():
        records[f"direct/{key}"] = _safe(lambda: _describe(make(), ioh, f"direct/{key}", 3))

    for name, (pids, instances, dims) in SUITES.items():
        records[f"suite/{name}"] = _safe(lambda: _describe_suite(
            getattr(ioh.iohcpp.suite, name)(pids, instances, dims)))
    return records


def _describe_suite(suite):
    sequence = []
    for problem in suite:
        meta = problem.meta_data
        sequence.append([int(meta.problem_id), str(meta.name), int(meta.instance), int(meta.n_variables)])
    return {
        "name": str(suite.name),
        "len": len(suite),
        "problem_ids": [int(v) for v in suite.problem_ids],
        "instances": [int(v) for v in suite.instances],
        "dimensions": [int(v) for v in suite.dimensions],
        "sequence": sequence,
    }


def union_pairs(registry):
    """(union ProblemClass, id, family) for every id of REAL and INTEGER."""
    pairs = []
    families = registry["problem_classes"]
    dbv = registry["views"].get("DynamicBinVal", {})
    for union, members in (("REAL", ("BBOB", "CEC2013", "CEC2022", "STAR_REAL")),
                           ("INTEGER", ("PBO", "GRAPH", "STAR_INTEGER"))):
        for pid, name in families[union].items():
            family = next((f for f in members if families[f].get(pid) == name), None)
            if family is None and dbv.get(pid) == name:
                family = "DynamicBinVal"
            pairs.append((union, int(pid), family))
    return pairs


def assert_close(test, expected, actual, path="root"):
    """Recursive comparison: names/ints exact, floats with REL_TOL/ABS_TOL, containers by value."""
    if isinstance(expected, dict):
        test.assertIsInstance(actual, dict, path)
        test.assertEqual(sorted(expected), sorted(actual), f"{path}: keys differ")
        for k in expected:
            assert_close(test, expected[k], actual[k], f"{path}/{k}")
    elif isinstance(expected, list):
        actual = list(actual)
        test.assertEqual(len(expected), len(actual), f"{path}: length differs")
        for i, (e, a) in enumerate(zip(expected, actual)):
            assert_close(test, e, a, f"{path}[{i}]")
    elif isinstance(expected, float) or isinstance(actual, float):
        e, a = float(expected), float(actual)
        if math.isnan(e) or math.isnan(a):
            test.assertTrue(math.isnan(e) and math.isnan(a), f"{path}: {e} != {a}")
        elif math.isinf(e) or math.isinf(a):
            test.assertEqual(e, a, path)
        else:
            test.assertTrue(math.isclose(e, a, rel_tol=REL_TOL, abs_tol=ABS_TOL), f"{path}: {e!r} != {a!r}")
    else:
        test.assertEqual(expected, actual, path)


def fresh_registry_snapshot():
    """Run --dump-registry in a fresh interpreter (other tests register wrapped problems in-process)."""
    import subprocess
    out = subprocess.run([sys.executable, os.path.abspath(__file__), "--dump-registry"],
                         check=True, capture_output=True, text=True).stdout
    return json.loads(out)


def load(path):
    with open(path) as f:
        return json.load(f)


def main(argv):
    import ioh

    if "--dump-registry" in argv:
        snapshot = registry_snapshot(ioh)
        snapshot["wrap"] = wrap_scenario(ioh)
        json.dump(snapshot, sys.stdout, sort_keys=True)
        return 0
    if "--write" in argv:
        os.makedirs(FIXTURE_DIR, exist_ok=True)
        registry = registry_snapshot(ioh)
        behaviour = behaviour_records(ioh, registry)
        registry["wrap"] = wrap_scenario(ioh)  # after the snapshot: it adds entries to REAL/INTEGER
        with open(REGISTRY_FILE, "w") as f:
            json.dump(registry, f, indent=1, sort_keys=True)
        with open(BEHAVIOUR_FILE, "w") as f:  # one record per line keeps regeneration diffs readable
            lines = [f"{json.dumps(k)}: {json.dumps(behaviour[k], sort_keys=True)}" for k in sorted(behaviour)]
            f.write("{\n" + ",\n".join(lines) + "\n}\n")
        print(f"wrote {REGISTRY_FILE} and {BEHAVIOUR_FILE} ({len(behaviour)} records)")
        return 0
    print(__doc__)
    return 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
