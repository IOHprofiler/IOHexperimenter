"""Machine-readable registry of the suites of IOHexperimenter.

``ioh.registry()`` returns a dict (JSON compatible) with every suite, its problems and the conventions a
leaderboard needs. The structure of the problems (ids, names, direction, bounds, optima) is read from the problem
classes of the installed package, so it cannot drift from the C++ code. The conventions (limits of instances and
dimensions, standard budget, standard instances, target bounds and scoring method) are listed in ``SUITE_CONVENTIONS``
below. ``python -m ioh.suite_registry [-o FILE]`` writes the registry as JSON (the release workflow attaches it to each
release).

Scoring is configuration, not code: every suite has a scoring method (AOCC to start with), and a problem can override
it in ``SCORING_OVERRIDES``. ``scoring(suite, problem_id)`` gives the method that applies.
"""

import argparse
import copy
import json
import math
import sys
import typing
from importlib import metadata

from . import ProblemClass, get_problem, OptimizationType

SCHEMA_VERSION = 1

#: The scoring method of every suite. AOCC: the area over the convergence curve.
DEFAULT_SCORING: typing.Dict[str, typing.Any] = {"method": "AOCC", "parameters": {}}

#: Scoring per problem, ``{suite name: {problem id: {"method": ..., "parameters": {...}}}}``; a problem that is not
#: listed uses the scoring of its suite.
SCORING_OVERRIDES: typing.Dict[str, typing.Dict[int, typing.Dict[str, typing.Any]]] = {}

_LOG_PRECISION = {"scale": "log", "relative_to_optimum": True, "lower": 1e-8, "upper": 1e2}
_UNKNOWN_TARGETS = {"scale": "linear", "relative_to_optimum": False, "lower": None, "upper": None}

#: Per suite: the class of its problems, the largest instance and dimension its C++ class accepts (these are checked
#: against include/ioh/suite.hpp by the tests), and the conventions of a leaderboard. A budget is
#: ``base + per_dimension * dimension`` evaluations. ``target_bounds`` are the range of target values that scoring
#: considers: ``relative_to_optimum`` ones are precisions (distance to the optimum), the others are function values.
#: ``None`` means that the convention is not settled for that suite.
SUITE_CONVENTIONS: typing.Dict[str, typing.Dict[str, typing.Any]] = {
    "BBOB": {
        "problem_class": ProblemClass.BBOB,
        "instances": {"min": 1, "max": 100},
        "dimensions": {"min": 2, "max": 100},
        "standard_instances": [1, 2, 3, 4, 5],
        "standard_dimensions": [2, 3, 5, 10, 20, 40],
        "budget": {"base": 0, "per_dimension": 10000},
        "target_bounds": _LOG_PRECISION,
    },
    "SBOX": {
        "problem_class": ProblemClass.SBOX,
        "instances": {"min": 1, "max": 100},
        "dimensions": {"min": 2, "max": 100},
        "standard_instances": [1, 2, 3, 4, 5],
        "standard_dimensions": [2, 3, 5, 10, 20, 40],
        "budget": {"base": 0, "per_dimension": 10000},
        "target_bounds": _LOG_PRECISION,
    },
    "PBO": {
        "problem_class": ProblemClass.PBO,
        "instances": {"min": 1, "max": 100},
        "dimensions": {"min": 1, "max": 20000},
        "standard_instances": [1, 2, 3, 4, 5],
        "standard_dimensions": [16, 64, 100],
        "budget": {"base": 0, "per_dimension": 100},
        "target_bounds": {"scale": "linear", "relative_to_optimum": True, "lower": 0.0, "upper": None},
    },
    "Submodular": {
        "problem_class": ProblemClass.GRAPH,
        "instances": {"min": 1, "max": 1},
        "dimensions": {"min": 1, "max": 1},
        "standard_instances": [1],
        "standard_dimensions": [1],
        "budget": None,
        "target_bounds": _UNKNOWN_TARGETS,
    },
    "RealStarDiscrepancy": {
        "problem_class": ProblemClass.STAR_REAL,
        "instances": {"min": 1, "max": 10000},
        "dimensions": {"min": 1, "max": 10000},
        "standard_instances": [1],
        "standard_dimensions": [5],
        "budget": None,
        "target_bounds": _UNKNOWN_TARGETS,
    },
    "IntegerStarDiscrepancy": {
        "problem_class": ProblemClass.STAR_INTEGER,
        "instances": {"min": 1, "max": 10000},
        "dimensions": {"min": 1, "max": 10000},
        "standard_instances": [1],
        "standard_dimensions": [5],
        "budget": None,
        "target_bounds": _UNKNOWN_TARGETS,
    },
}

#: Dimensions of problems that need a perfect square (checked by get_problem)
_SQUARE_DIMENSION = {"PBO": {21, 23}}


def _number(value: float) -> typing.Optional[float]:
    """A finite number, or None for NaN and infinity (an optimum that is not known)."""
    value = float(value)
    return value if math.isfinite(value) else None


def _optimum(problem_class: ProblemClass, fid: int, instance: int, dimension: int):
    return _number(get_problem(fid, instance, dimension, problem_class).optimum.y)


def _problem_entry(
    suite_name: str, problem_class: ProblemClass, fid: int, name: str, conventions: dict
) -> typing.Dict[str, typing.Any]:
    dims = conventions["dimensions"]
    reference_dimension = conventions["standard_dimensions"][0]
    if fid in _SQUARE_DIMENSION.get(suite_name, ()):
        reference_dimension = 16
    problem = get_problem(fid, 1, reference_dimension, problem_class)
    meta = problem.meta_data
    # the graph problems ignore the requested dimension: each has its own
    fixed_dimension = int(meta.n_variables) if suite_name == "Submodular" else None
    other_dimension = min(reference_dimension + 1, dims["max"])

    y = _number(problem.optimum.y)
    # does the optimal value change with the instance or the dimension? Compared on a second instance (when the
    # suite has one) and a second dimension
    instance_dependent = False
    dimension_dependent = False
    if y is not None:
        if conventions["instances"]["max"] > 1:
            instance_dependent = _optimum(problem_class, fid, 2, reference_dimension) != y
        if fixed_dimension is None and fid not in _SQUARE_DIMENSION.get(suite_name, ()):
            dimension_dependent = _optimum(problem_class, fid, 1, other_dimension) != y

    return {
        "id": fid,
        "name": name,
        "optimization_type": "MAX" if meta.optimization_type == OptimizationType.MAX else "MIN",
        "maximization": meta.optimization_type == OptimizationType.MAX,
        "bounds": {"lower": float(problem.bounds.lb[0]), "upper": float(problem.bounds.ub[0])},
        "fixed_dimension": fixed_dimension,
        "dimension_requirement": "perfect_square" if fid in _SQUARE_DIMENSION.get(suite_name, ()) else None,
        "known_optimum": y is not None,
        "optimum": {
            "instance": 1,
            "dimension": reference_dimension if fixed_dimension is None else fixed_dimension,
            "value": y,
            "depends_on_instance": instance_dependent,
            "depends_on_dimension": dimension_dependent,
        },
        # None: the scoring of the suite applies
        "scoring": copy.deepcopy(SCORING_OVERRIDES.get(suite_name, {}).get(fid)),
    }


def _build() -> typing.Dict[str, typing.Any]:
    suites = {}
    for name, conventions in SUITE_CONVENTIONS.items():
        problem_class = conventions["problem_class"]
        entry = {
            "name": name,
            "problem_class": problem_class.name,
            "variable_type": "real" if problem_class.is_real() else "integer",
            "instances": dict(conventions["instances"]),
            "dimensions": dict(conventions["dimensions"]),
            "standard_instances": list(conventions["standard_instances"]),
            "standard_dimensions": list(conventions["standard_dimensions"]),
            "budget": copy.deepcopy(conventions["budget"]),
            "target_bounds": copy.deepcopy(conventions["target_bounds"]),
            "scoring": copy.deepcopy(DEFAULT_SCORING),
            "problems": [
                _problem_entry(name, problem_class, fid, pname, conventions)
                for fid, pname in sorted(problem_class.problems.items())
            ],
        }
        suites[name] = entry
    try:
        version = metadata.version("ioh")
    except metadata.PackageNotFoundError:
        version = None
    return {"schema_version": SCHEMA_VERSION, "ioh_version": version, "suites": suites}


_cache: typing.Optional[typing.Dict[str, typing.Any]] = None


def registry() -> typing.Dict[str, typing.Any]:
    """The registry of all suites, a JSON compatible dict (a copy: change it freely).

    ``registry()["suites"][name]`` holds, per suite: the ``problem_class``, the ``instances`` and ``dimensions``
    ranges that the suite accepts, ``standard_instances`` and ``standard_dimensions``, the standard ``budget``
    (``base + per_dimension * dimension`` evaluations, None when not settled), ``target_bounds``, the ``scoring``
    method and the list of ``problems``. A problem has its ``id``, ``name``, ``optimization_type`` ("MIN" or "MAX"),
    ``maximization``, ``bounds``, ``fixed_dimension`` (for problems that ignore the requested dimension),
    ``known_optimum``, ``optimum`` (the value of the optimum of instance 1 in the reference dimension, and whether it
    depends on the instance or the dimension) and a scoring override (``scoring``, None for the suite's).
    """
    global _cache
    if _cache is None:
        _cache = _build()
    return copy.deepcopy(_cache)


def scoring(suite_name: str, problem_id: typing.Optional[int] = None) -> typing.Dict[str, typing.Any]:
    """The scoring method for a suite, or for one of its problems.

    A problem's own override applies when it has one, else the suite's.
    """
    entry = registry()["suites"][suite_name]
    if problem_id is not None:
        for problem in entry["problems"]:
            if problem["id"] == problem_id:
                return problem["scoring"] or entry["scoring"]
        raise KeyError(f"{suite_name} has no problem with id {problem_id}")
    return entry["scoring"]


def to_json(indent: typing.Optional[int] = 2) -> str:
    """The registry as JSON text."""
    return json.dumps(registry(), indent=indent, allow_nan=False) + "\n"


def main(argv: typing.Optional[typing.Sequence[str]] = None) -> int:
    parser = argparse.ArgumentParser(prog="python -m ioh.suite_registry", description="Write the registry as JSON.")
    parser.add_argument("-o", "--output", help="the file to write (default: standard output)")
    args = parser.parse_args(argv)
    text = to_json()
    if args.output:
        with open(args.output, "w", encoding="utf-8", newline="\n") as f:
            f.write(text)
    else:
        sys.stdout.write(text)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
