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
    :include: PBO
    :include: OneMax
    :include: LeadingOnes
    :include: Linear
    :include: OneMaxDummy1
    :include: OneMaxDummy2
    :include: OneMaxNeutrality
    :include: OneMaxEpistasis
    :include: OneMaxRuggedness1
    :include: OneMaxRuggedness2
    :include: OneMaxRuggedness3
    :include: LeadingOnesDummy1
    :include: LeadingOnesDummy2
    :include: LeadingOnesNeutrality
    :include: LeadingOnesEpistasis
    :include: LeadingOnesRuggedness1
    :include: LeadingOnesRuggedness2
    :include: LeadingOnesRuggedness3
    :include: LABS
    :include: IsingRing
    :include: IsingTorus
    :include: IsingTriangular
    :include: MIS
    :include: NQueens
    :include: ConcatenatedTrap
    :include: NKLandscape
   

Reference
---------
[DoerrYHWSB20] Carola Doerr, Furong Ye, Naama Horesh, Hao Wang, Ofer M. Shir, and Thomas Bäck.
"Benchmarking discrete optimization heuristics with IOHprofiler." Applied Soft Computing 88 (2020): 106027.

[WeiseW18] Thomas Weise and Zijun Wu. "Difficult features of combinatorial optimization problems and
the tunable w-model benchmark problem for simulating them." In Proceedings of the Genetic
and Evolutionary Computation Conference Companion, pp. 1769-1776. 2018.
