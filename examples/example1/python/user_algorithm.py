""" Implement your algorithm in this file.
"""
import numpy as np 

""" The maximal number of independent restarts allowed for an algorithm that restarts itself.
"""
independent_restart = 1

""" The maximal budget for evaluations done by an optimization algorithm equals dimension * budget.
    Increase the budget multiplier value gradually to see how it affects the runtime.
"""
budget = 2 


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
    """ This is a random serach method."""
    lbounds, ubounds = np.array(lbounds), np.array(ubounds)
    dim, best_value = fun.dimension, 0
    while budget > 0:
        X = lbounds + (ubounds - lbounds + 1) * np.random.rand(1, dim)
        X = X.astype(int);
        para = np.array([budget])
        fun.set_parameters(para)
        F = [fun(x) for x in X]
        if fun.number_of_objectives == 1:
            index = np.argmin(F)
            if best_value is None or F[index] > best_value:
                best_value = F[index]
        budget -= 1
    return best_value