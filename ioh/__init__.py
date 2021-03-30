try:
    from .iohcpp import *
except ModuleNotFoundError:
    raise ModuleNotFoundError("No module named ioh")

def get_problem(fid, iid, dim, suite = "BBOB"):
    '''Instansiate a problem based on its function ID, dimension, instance and suite
    Parameters
    ----------
    fid:
        The function ID of the problem in the suite, or the name of the function as string
    dim:
        The dimension (number of variables) of the problem
    iid:
        The instance ID of the problem
    target_precision:
        Optional, how close to the optimum the problem is considered 'solved'
    suite:
        Which suite the problem is from. Either 'BBOB' or 'PBO'. Only used if fid is an integer
    '''
    if suite == "BBOB":
        return problem.Real.factory().create(fid, iid, dim)
    elif suite == "PBO":
        if fid in [21, 23]:
            if not np.sqrt(dim).is_integer():
                raise Exception("For this function, the dimension needs to be a perfect square!")
        return problem.Integer.factory().create(fid, iid, dim)
    else:
        raise Exception("This suite is not yet supported")
    
