""" Implement your algorithm in this file.
"""
import numpy as np 
import random as rd
import math

""" The maximal number of independent restarts allowed for an algorithm that restarts itself.
"""
independent_restart = 1

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


def mutation(ind,mutation_rate,dim):
    l = 0
    while l == 0:
        l = np.random.binomial(dim,mutation_rate)
    flip = rd.sample(range(0,dim),l)
    for index in flip:
        ind[index] = (ind[index] + 1) % 2

    return l


def user_algorithm(fun,lbounds,ubounds,budget):
    """ This is a one plus lambda EA."""
    lbounds, ubounds = np.array(lbounds), np.array(ubounds)
    dim = fun.dimension
    parent = lbounds + (ubounds - lbounds + 1) * np.random.rand(dim)
    parent = parent.astype(int)

    best = parent.copy()
    budget -= 1
    lamb = 1
    mutation_rate = 1.0/dim
    para = np.array([mutation_rate])
    fun.set_parameters(para)
    best_value = fun(parent)
    while budget > 0:
        for i in range(0,lamb):
            offspring = parent.copy()
            l = mutation(offspring,mutation_rate,dim)
            para = np.array([mutation_rate,l])
            fun.set_parameters(para)
            v = fun(offspring)
            if(v > best_value):
                best_value = v
                best = offspring.copy()
            budget -= 1
            if(budget == 0):
                break
        parent = best.copy()
        mutation_rate = 1.0 / (1 + (1 - mutation_rate) / mutation_rate * math.exp(0.22 * np.random.normal(0,1)))
        mutation_rate = min(max(mutation_rate,1.0/dim),0.5)
    return best_value