import numpy as np
from IOHexperimenter import IOH_function, IOH_logger

def mutation(parent, l):
    n = len(parent)
    index = np.random.choice(range(n), l, replace=False)
    offspring = parent.copy()
    offspring[index] = ~offspring[index]
    return offspring

def one_plus_one_EA(problem, target_hit, budget=1e3):
    n = problem.f.IOHprofiler_get_number_of_variables()
    x = np.random.rand(n) < 0.5
    x_prime = np.zeros(n, dtype=np.bool)

    fopt = problem(x)
    counter = 0
    l = 1

    while not target_hit and counter < budget:
        x_prime[:] = mutation(x, l)
        f = problem(1 * x_prime)

        if f >= fopt:
            x[:] = x_prime
            fopt = f

        counter += 1
    return x, fopt

if __name__ == '__main__':
    runs = 10

    for fid in range(1, 2):
        f = IOH_function(fid = fid, dim = 16, iid = 1, suite = 'BBOB')
        logger = IOH_logger(
            location = "./data", foldername = "data", 
            name = "one_plu_one_EA", 
            info = "test of IOHexperimenter in python"
        )
        f.add_logger(logger)
        
        print('F%d '%fid + '...')
        for i in range(runs):
            print(' ' + 'run {}/{}'.format(i + 1, runs) + '...', end=' ')
            xopt, fopt = one_plus_one_EA(f, f.final_target_hit)
            print('xopt: {}, fopt: {:.2f}'.format(1 * xopt, fopt))
            f.reset()
        f.clear_logger()
