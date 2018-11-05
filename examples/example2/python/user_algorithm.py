""" Implement your algorithm in this file.
"""
import numpy as np 

""" The maximal number of independent restarts allowed for an algorithm that restarts itself.
"""
independent_restart = 10

""" The maximal budget for evaluations done by an optimization algorithm equals dimension * budget.
    Increase the budget multiplier value gradually to see how it affects the runtime.
"""
budget = 50 


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


def mutation(ind,mutate_rate):
    flag = 0
    while flag == 0:
        for i in range(0,len(ind)):
            if np.random.random() < mutate_rate:
                ind[i] = (ind[i] + 1) % 2
                flag = flag + 1
    return flag


def user_algorithm(fun,lbounds,ubounds,budget):
    """ This is a one plus lambda EA."""
    lbounds, ubounds = np.array(lbounds), np.array(ubounds)
    dim = fun.dimension
    parent = lbounds + (ubounds - lbounds + 1) * np.random.rand(dim)
    parent = parent.astype(int)
    best = parent.copy()
    budget -= 1
    lamb = 1
    para = np.array([1.0/dim,lamb])
    fun.set_parameters(para)
    best_value = fun(parent)
    while budget > 0:
        for i in range(0,lamb):
            offspring = parent.copy()
            l = mutation(offspring,1.0/dim)
            para = np.array([1.0/dim,lamb])
            fun.set_parameters(para)
            f = fun(offspring)
            if f > best_value :
                best_value = f
                best = offspring.copy()
            budget -= 1
            if(budget == 0):
                break
        parent = best.copy()
    return best_value