""" Implement your algorithm in this file.
"""
import numpy as np

""" The maximal number of independent restarts allowed for an algorithm that restarts itself.
"""
independent_restart = 5

""" The maximal budget for evaluations done by an optimization algorithm equals dimension * budget.
    Increase the budget multiplier value gradually to see how it affects the runtime.
"""
budget = 100


"""
Select one individual out of a population Genome with fitness values fitness using proportional selection
"""
def select_proportional(Genome, fitness, rand_state):
    cumsum_f = np.cumsum(fitness)
    minParent = min(cumsum_f)
    if minParent < 0:
        cumsum_f += (minParent * -1)
    r = sum(fitness) * rand_state.uniform()
    idx = np.ravel(np.where(r < cumsum_f))[0]
    return Genome[:, [idx]]

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

    eval_cntr = 0
    n=fun.dimension
    mu = 1000
    pc = 0.5
    pm = 4 / n
    local_state = np.random.RandomState()
    Genome = local_state.randint(2, size=(n,mu))
    fitness=[]
    for k in range(mu):
        fitness.append(fun(Genome[:, k]))

    eval_cntr += mu
    fmax = np.max(fitness)
    while (eval_cntr < budget):
        #       Generate offspring population (recombination, mutation)
        newGenome = np.empty((n, mu), dtype=int)
        #        1. sexual selection + 1-point recombination
        for i in range(int(mu / 2)):

            p1 = select_proportional(Genome, fitness, local_state)
            p2 = select_proportional(Genome, fitness, local_state)
            if local_state.uniform() < pc:  # recombination
                idx = local_state.randint(n, dtype=int)
                Xnew1 = np.concatenate((p1[:idx], p2[idx:]))
                Xnew2 = np.concatenate((p2[:idx], p1[idx:]))
            else:  # no recombination; two parents are copied as are
                Xnew1 = p1
                Xnew2 = p2
            # 2. mutation
            mut1_bits = local_state.uniform(size=(n, 1)) < pm
            mut2_bits = local_state.uniform(size=(n, 1)) < pm

            Xnew1[mut1_bits] = 1 - Xnew1[mut1_bits]
            Xnew2[mut2_bits] = 1 - Xnew2[mut2_bits]
            #
            newGenome[:, [2 * i - 1]] = Xnew1
            newGenome[:, [2 * i]] = Xnew2
        # The best individual of the parental population is kept
        newGenome[:, [mu - 1]] = Genome[:, [np.argmax(fitness)]]
        Genome = newGenome
        fitness.clear()
        for k in range(mu):
            fitness.append(fun(Genome[:, k]))
        eval_cntr += mu
        fcurr_best = np.max(fitness)
        if fmax < fcurr_best:
            fmax = fcurr_best

    return fmax



