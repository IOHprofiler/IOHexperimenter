import IOHexperimenter as IOH
import numpy as np

def mutation(parent, l):
    n = len(parent)
    index = np.random.choice(range(n), l, replace=False)
    offspring = parent.copy()
    offspring[index] = ~offspring[index]
    return offspring

def one_plus_one_EA(problem, logger, budget=1e3):
    n = problem.IOHprofiler_get_number_of_variables()
    x = np.random.rand(n) < 0.5
    x_prime = np.zeros(n, dtype=np.bool)

    fopt = problem.evaluate(x)
    logger.write_line(problem.loggerInfo())
    counter = 0
    l = 1

    while not problem.IOHprofiler_hit_optimal() and counter < budget:
        x_prime[:] = mutation(x, l)
        f = problem.evaluate(1 * x_prime)
        logger.write_line(problem.loggerInfo())

        if f >= fopt:
            x[:] = x_prime
            fopt = f

        counter += 1
    return x, fopt

if __name__ == '__main__':
    p_id = range(1, 24)
    i_id = [1]
    dimension = [16]
    runs = 10

    pbo = IOH.PBO_suite(p_id, i_id, dimension)
    pbo.loadProblem()

    logger = IOH.IOHprofiler_csv_logger("./", "data", "EA", "EA")
    logger.set_interval(0)
    logger.activate_logger()
    logger.track_suite(pbo.IOHprofiler_suite_get_suite_name())

    p = pbo.get_next_problem()
    k = 1
    while p:
        print('F%d '%k + p.IOHprofiler_get_problem_name() + '...')
        for i in range(runs):
            p = pbo.get_current_problem()
            logger.track_problem(
                p.IOHprofiler_get_problem_id(),
                p.IOHprofiler_get_number_of_variables(),
                p.IOHprofiler_get_instance_id(),
                p.IOHprofiler_get_problem_name(),
                p.IOHprofiler_get_optimization_type())

            print(' ' + 'run {}/{}'.format(i + 1, runs) + '...', end=' ')
            xopt, fopt = one_plus_one_EA(p, logger)
            print('xopt: {}, fopt: {:.2f}'.format(1 * xopt, fopt))

        p = pbo.get_next_problem()
        k += 1
