import IOHexperimenter as IOH
import numpy as np

def mutation(parent, l):
    n = len(parent)
    flip = np.random.choice(range(n), l, replace=False)

    offspring = parent.copy()
    for i in flip:
        offspring[i] = (parent[i] + 1) % 2

    return offspring.copy()

def one_plus_one_EA(problem, logger):
    n = problem.IOHprofiler_get_number_of_variables()
    x = np.zeros(n)
    x_prime = np.zeros(n)

    for i in range(n):
        if np.random.rand() < 0.5:
            x[i] = 1

    fopt = problem.evaluate(x)
    logger.write_line(problem.loggerInfo())
    counter = 0
    l = 1

    while not problem.IOHprofiler_hit_optimal() and counter < 1000:
        x_prime[:] = mutation(x, l)
        f = problem.evaluate(x_prime)
        logger.write_line(problem.loggerInfo())

        if f >= fopt:
            x[:] = x_prime.copy()
            fopt = f

        print(x)
        counter += 1

    return x, fopt

if __name__ == '__main__':
    p_id = range(1,24)
    i_id = range(1,10)
    dimension = [16]
    runs = 10

    pbo = IOH.PBO_suite(p_id, i_id, dimension)
    pbo.loadProblem()
    logger = IOH.IOHprofiler_csv_logger("./", "run_suite", "EA", "EA")
    logger.set_interval(0)
    logger.activate_logger()
    logger.track_suite(pbo.IOHprofiler_suite_get_suite_name())

    p = pbo.get_next_problem()
    while p:
        while runs > 0:
            p = pbo.get_current_problem()
            logger.track_problem(
                p.IOHprofiler_get_problem_id(),
                p.IOHprofiler_get_number_of_variables(),
                p.IOHprofiler_get_instance_id(),
                p.IOHprofiler_get_problem_name(),
                p.IOHprofiler_get_optimization_type()
            )

            one_plus_one_EA(p, logger)
            runs -= runs

        p = pbo.get_next_problem()
