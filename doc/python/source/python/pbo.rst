PBO
================
Pseudo-Boolean Optimization (PBO) problem set, which contains 25 test functions taking their domain on {0, 1}^n, where n is the length of bitstrings.

In PBO, we cover some theory-motivated function, e.g., OneMax and LeadingOnes as well as others with more practical  relevance, e.g., the NK Landscape [DoerrYHWSB20].
We also utilized the so-called W-model for generating/enriching the problem set [WeiseW18].

Problems
--------

.. automodapi:: ioh.iohcpp.problem
    :inherited-members:
    :no-main-docstr:
    :no-heading:
    :skip: wrap_integer_problem, wrap_real_problem, AbstractWModel, AttractiveSector, BBOB, BentCigar, BuecheRastrigin, DifferentPowers, Discus, Ellipsoid, EllipsoidRotated, Gallagher101, Gallagher21, GriewankRosenBrock, Integer, IntegerWrappedProblem, Katsuura, LinearSlope, LunacekBiRastrigin, Rastrigin, RastriginRotated, Real, RealWrappedProblem, Rosenbrock, RosenbrockRotated, Schaffers10, Schaffers1000, Schwefel, SharpRidge, Sphere, StepEllipsoid, WModelLeadingOnes, WModelOneMax, Weierstrass

Reference
---------
[DoerrYHWSB20] Carola Doerr, Furong Ye, Naama Horesh, Hao Wang, Ofer M. Shir, and Thomas Bäck.
"Benchmarking discrete optimization heuristics with IOHprofiler." Applied Soft Computing 88 (2020): 106027.

[WeiseW18] Thomas Weise and Zijun Wu. "Difficult features of combinatorial optimization problems and
the tunable w-model benchmark problem for simulating them." In Proceedings of the Genetic
and Evolutionary Computation Conference Companion, pp. 1769-1776. 2018.
