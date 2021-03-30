import math
from ioh import problem, suite, experimenter, logger, OptimizationType

f1 = problem.Real.factory()
f2 = problem.Integer.factory()

expected = [
    161.17445568,
    12653420.412225708,
    -97.06158297486468,
    8845.53524810093,
    121.9133638725643,
    423021.00682286796,
    278.3290201933729,
    4315.032550201522,
    95168.253629631741,
    969025.0451803299,
    20911310.769634742,
    187251637.66430587,
    2198.7155122256763,
    261.60997479957,
    1103.1553337856833,
    273.87870697835791,
    35.410739487431208,
    107.64134358767826,
    -40.269198932753994,
    49739.05388887795,
    116.29585727504872,
    -914.902473409051,
    18.635078550302751,
    1782.2733296400438,
]

for i in sorted(f1.ids()):
    p = f1.create(i, 1, 5)
    print(p)
    assert math.isclose(p([0.1, 1., 2., 4., 5.4])[0], expected[i-1]), pn
    

for i in sorted(f2.ids()):
    p = f2.create(i, 1, 4)
    print(p)


class XProblem(problem.Real):
    def __init__(self):
        super().__init__(self.__class__.__name__)

    
# x = XProblem() # you have to call this before being registered

import random


def algorithm(p: problem.Real):
    for _ in range(100):
        x = list(map(lambda x: random.random(), range(p.meta_data.n_variables)))    
        p(x)


suite = suite.BBOB(list(range(1, 25)), [5, 10], [5])
l1 = logger.Default("data", store_positions=True)
l2 = logger.ECDF(0, 6e7, 20, 0, 100, 20)
logger = logger.LoggerCombine([l1, l2])

e = experimenter.Real(suite, logger, algorithm, 5)
e.run()



class A:
    x = 1
    
    def run():
        x +=1 

a = A()

# logger.Default(, parameters_object=a, tracked_parameters=["x"])

# def plot_ecdf(e):
#     for row in e:
#         print(" ".join(map(str, map(int, row))))

# plot_ecdf(l2.at(1, 5, 5))

p = problem.Real.factory().create(1, 5, 5)

"""" 
    TODO: IOH_function wrapper maken
        def get_problem(suite_name, *args, **kwargs):
            return getattr(problem, suite_name).factory().create(*args, **kwargs)
    TODO: Check quadratic function, min dimension bbob=2
    TODO: Betere error handling
    TODO: Rename Integer -> Discrete
    TODO: logger.Default(, parameters_object=bla, tracked_parameters=[]) getattr(bla, name)
    TODO: bounds aanpasses voor wrappedproblem
    TODO: check wat er gebeurt met naam initializatie van wrapped problem
    TODO: check wat er op de exit gebeurt met de exit
""""


# def f(x, functie_object):
#     functie_object


