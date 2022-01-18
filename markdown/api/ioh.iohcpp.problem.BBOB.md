# BBOB


### _class_ ioh.iohcpp.problem.BBOB()
Bases: [`ioh.iohcpp.problem.Real`](ioh.iohcpp.problem.Real.md#ioh.iohcpp.problem.Real)

Black-Box Optimization Benchmarking (BBOB) problem set, which contains 24 noiseless
real-valued test functions supported on [-5, 5]^n, where n is the dimensionality.

This problem was orginally proposed by Hansen et. al. in [FinckHRA10] and was implemented
as the core component of the COmparing Continous Optimizer (COCO) platform [HansenARMTB20].

We took the implementation of those 24 functions in
[https://github.com/numbbo/coco/tree/master/code-experiments/src](https://github.com/numbbo/coco/tree/master/code-experiments/src) (v2.2)
and adopted those to our framework.

We have acknowledged and specified in our license file
[https://github.com/IOHprofiler/IOHexperimenter/blob/master/LICENSE.md](https://github.com/IOHprofiler/IOHexperimenter/blob/master/LICENSE.md)
the usage and modification to the COCO/BBOB sources.

[HansenARMTB20] Nikolaus Hansen, Anne Auger, Raymond Ros, Olaf Mersmann,
Tea Tusar, and Dimo Brockhoff. “COCO: A platform for comparing continuous optimizers in
a black-box setting.” Optimization Methods and Software (2020): 1-31.

[FinckHRA10] Steffen Finck, Nikolaus Hansen, Raymond Ros, and Anne Auger.
“Real-parameter black-box optimization benchmarking 2009: Presentation of the noiseless functions.”
Technical Report 2009/20, Research Center PPE, 2009. Updated February, 2010.
