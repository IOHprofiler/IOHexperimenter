from functools import partial
import src.IOH_Config as Config

def runParallelFunction(runFunction, arguments, force_single = False, timeout = 30):
    """
        Return the output of runFunction for each set of arguments,
        making use of as much parallelization as possible on this system

        :param runFunction: The function that can be executed in parallel
        :param arguments:   List of tuples, where each tuple are the arguments
                            to pass to the function
        :param force_single:Force the run to be single-threaded, ignoring all settings from IOH_config
        :param timeout:     How many seconds each execution get before being stopped. 
                            Only used when running in parallel without mpi.
        :return:
    """
    if force_single:
        return runSingleThreaded(runFunction, arguments)    
    if Config.evaluate_parallel:
        if Config.use_MPI:
            return runMPI(runFunction, arguments)
        elif Config.use_pebble:
            return runPebblePool(runFunction, arguments, timeout = timeout)
        else:
            return runPool(runFunction, arguments)
    else:
        return runSingleThreaded(runFunction, arguments)


# Inline function definition to allow the passing of multiple arguments to 'runFunction' through 'Pool.map'
def func_star(a_b, func):
    """Convert `f([1,2])` to `f(1,2)` call."""
    return func(*a_b)


def runPool(runFunction, arguments):
    """
        Small overhead-function to handle multi-processing using Python's built-in multiprocessing.Pool

        :param runFunction: The (``partial``) function to run in parallel, accepting ``arguments``
        :param arguments:   The arguments to passed distributedly to ``runFunction``
        :return:            List of any results produced by ``runFunction``
    """
    print(f"Running pool with {Config.num_threads} threads")
    from multiprocessing import Pool
    p = Pool(Config.num_threads)

    local_func = partial(func_star, func=runFunction)
    results = p.map(local_func, arguments)
    p.close()
    return results

def runPebblePool(runfunction, arguments, timeout = 30):
    from pebble import ProcessPool
    from concurrent.futures import TimeoutError
    
    arguments = list(arguments)
    
    print(f"Running pebble pool with {min(Config.num_threads, len(arguments))} threads")

    
    def task_done(future):
        try:
            result = future.result()  # blocks until results are ready
        except TimeoutError as error:
            print("Function took longer than %d seconds" % error.args[1])
        except Exception as error:
            print("Function raised %s" % error)
            print(error)  # traceback of the function
            
    with ProcessPool(max_workers = min(Config.num_threads, len(arguments)), max_tasks = len(arguments)) as pool:
        for x in arguments:
            if type(x) is not list and type(x) is not tuple:
                x = [x]
            future = pool.schedule(runfunction, args=x, timeout=timeout)
            future.add_done_callback(task_done)

def runSingleThreaded(runFunction, arguments):
    """
        Small overhead-function to iteratively run a function with a pre-determined input arguments

        :param runFunction: The (``partial``) function to run, accepting ``arguments``
        :param arguments:   The arguments to passed to ``runFunction``, one run at a time
        :return:            List of any results produced by ``runFunction``
    """
    results = []
    print("Running single-threaded")
    for arg in arguments:
        results.append(runFunction(*arg))
    return results


def runMPI(runFunction, arguments):
    from schwimmbad import MPIPool
    print("Running using MPI")
    with MPIPool() as pool:
        results = pool.map(runFunction, arguments)
    return results
  
