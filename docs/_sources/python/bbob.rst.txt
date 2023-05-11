BBOB & SBOX
============
Black-Box Optimization Benchmarking (BBOB) problem set, which contains 24 noiseless real-valued test functions supported on [-5, 5]^n, where n is the dimensionality.

These problems were orginally proposed by Hansen et. al. in [FinckHRA10] and was implemented as the core component of the COmparing Continous Optimizer (COCO) platform [HansenARMTB20].

We took the implementation of those 24 functions in https://github.com/numbbo/coco/tree/master/code-experiments/src (v2.2) and adopted those to our framework.

We have acknowledged and specified in our license file https://github.com/IOHprofiler/IOHexperimenter/blob/master/LICENSE.md the usage and modification to the COCO/BBOB sources.


.. automodapi:: ioh.iohcpp.problem
    :inherited-members:
    :no-main-docstr:
    :no-heading:
    :include: BBOB
    :include: SBOX
    :include: Sphere
    :include: Ellipsoid
    :include: Rastrigin
    :include: BuecheRastrigin
    :include: LinearSlope
    :include: AttractiveSector
    :include: StepEllipsoid
    :include: Rosenbrock
    :include: RosenbrockRotated
    :include: EllipsoidRotated
    :include: Discus
    :include: BentCigar
    :include: SharpRidge
    :include: DifferentPowers
    :include: RastriginRotated
    :include: Weierstrass
    :include: Schaffers10
    :include: Schaffers1000
    :include: GriewankRosenbrock
    :include: Schwefel
    :include: Gallagher101
    :include: Gallagher21
    :include: Katsuura
    :include: LunacekBiRastrigin


Reference
---------
[HansenARMTB20] Nikolaus Hansen, Anne Auger, Raymond Ros, Olaf Mersmann,
Tea Tusar, and Dimo Brockhoff. "COCO: A platform for comparing continuous optimizers in
a black-box setting." Optimization Methods and Software (2020): 1-31.

[FinckHRA10] Steffen Finck, Nikolaus Hansen, Raymond Ros, and Anne Auger.
"Real-parameter black-box optimization benchmarking 2009: Presentation of the noiseless functions."
Technical Report 2009/20, Research Center PPE, 2009. Updated February, 2010. 