""" Implement your algorithm in this file.
"""

"""
The algorithm are from the package 'PySwarms' that can be found in the following link:
https://pyswarms.readthedocs.io/en/latest/intro.html

Miranda L.J., (2018). PySwarms: a research toolkit for Particle Swarm Optimization in Python. 
Journal of Open Source Software, 3(21), 433, https://doi.org/joss.00433
"""
import pyswarm

""" The maximal number of independent restarts allowed for an algorithm that restarts itself.
"""
independent_restart = 10

""" The maximal budget for evaluations done by an optimization algorithm equals dimension * budget.
    Increase the budget multiplier value gradually to see how it affects the runtime.
"""
budget = 100


""" An user defined algorithm.
    @param "fun" The class for evaluating variables' fitness. Achieving the fitness by nvoking the 
           statement `f(x)`.
    @param "lbounds" An array of variables' lower bounds.
    @param "ubounds" TAn array of variables' upper bounds.
    @param "budget" The maximal number of evaluations is equal to `budget` * dim. Dim is the dimension of the 
           problem, which can be acquired by fun.dimensionlen.
    @function "fun.set_parameters(para)" `para` is an array of parameters to be stored in output files.
              If users want to store parameters interested, please invoked this statement; otherwise, erase the 
              statement and no parameters will be stored.
"""

def user_algorithm(fun,lbounds,ubounds,budget):
    xopt, fopt = pyswarm.pso(fun,lbounds,ubounds, maxiter=budget)
    return fopt


