import IOHprofiler as IOH
import numpy as np
from copy import *

def mutation(parent, l):
    n = len(parent)
    flip = np.random.choice(range(n),l, replace=False)

    offspring = parent
    for i in flip:
        offspring[i] = (parent[i] + 1)%2


def EA(problem, logger):
    n = problem.IOHprofiler_get_number_of_variables()
    x = np.zeros(n)
    for i in range(n):
        if np.random.rand() < 0.5:
            x[i] = 1

    best_value = problem.evaluate(x)
    logger.write_line(problem.loggerInfo())
    counter = 0
    
    while(not problem.IOHprofiler_hit_optimal() and counter < 1000):
        p = 1./float(n)
  
      #Mutation phase
      #sample l from the binomial
      #l = np.random.binomial(n,p)
        l = 1
        x_prime = copy(x)
        mutation(x_prime,l)
        value = problem.evaluate(x_prime)
        logger.write_line(problem.loggerInfo())

        if value >= best_value:
            x = copy(x_prime)
            best_value = value
        counter += 1
    return best_value

def main():
    p_id = range(1,24)
    i_id = range(1,10)
    dimension = [16]
    pbo = IOH.PBO_suite(p_id, i_id, dimension)
    pbo.loadProblem()
    logger = IOH.IOHprofiler_csv_logger("./", "run_suite", "EA", "EA")
    logger.set_interval(0)
    logger.activate_logger()
    logger.target_suite(pbo.IOHprofiler_suite_get_suite_name())
    p = pbo.get_next_problem()
    while(p):
        runs = 10
        while(runs>0):
            p = pbo.get_current_problem()
            logger.target_problem(p.IOHprofiler_get_problem_id(), p.IOHprofiler_get_number_of_variables(),
                                  p.IOHprofiler_get_instance_id(), p.IOHprofiler_get_problem_name(),
                                  p.IOHprofiler_get_optimization_type())
            EA(p,logger)
            runs = runs-1
        p = pbo.get_next_problem()

if __name__ == "__main__":
    main()
