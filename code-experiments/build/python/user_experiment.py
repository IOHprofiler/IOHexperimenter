from __future__ import absolute_import, division, print_function, unicode_literals
try: range = xrange
except NameError: pass
import os, sys
import time
import numpy as np  # "pip install numpy" installs numpy
import IOHprofiler_python
from readcfg import configuration
from user_algorithm import user_algorithm,independent_restart,budget
from IOHprofiler_python import Suite, Observer, log_level, Problem
del absolute_import, division, print_function, unicode_literals

verbose = 1

def default_observers(update={}):
    _default_observers.update(update)
    return _default_observers
_default_observers = {
    'PBO': 'PBO',
    }

def default_observer_options(budget_=None, suite_name_=None):
    """return defaults computed from input parameters or current global vars
    """
    global budget, suite_name
    if budget_ is None:
        budget_ = budget
    if suite_name_ is None:
        suite_name_ = suite_name
    opts = {}
    try:
        opts.update({'result_folder': '%s'
                    %suite_name_})
    except: pass
    try:
        solver_module = '(%s)' % SOLVER.__module__
    except:
        solver_module = ''
    try:
        opts.update({'algorithm_name': SOLVER.__name__ + solver_module})
    except: pass
    return opts

class ObserverOptions(dict):
    """a `dict` with observer options which can be passed to
    the (C-based) `Observer` via the `as_string` property.

    Details: When the `Observer` class in future accepts a dictionary
    also, this class becomes superfluous and could be replaced by a method
    `default_observer_options` similar to `default_observers`.
    """
    def __init__(self, options={}):
        """set default options from global variables and input ``options``.

        Default values are created "dynamically" based on the setting
        of module-wide variables `SOLVER`, `suite_name`, and `budget`.
        """
        dict.__init__(self, options)
    def update(self, *args, **kwargs):
        """add or update options"""
        dict.update(self, *args, **kwargs)
        return self
    def update_gracefully(self, options):
        """update from each entry of parameter ``options: dict`` but only
        if key is not already present
        """
        for key in options:
            if key not in self:
                self[key] = options[key]
        return self
    @property
    def as_string(self):
        """string representation which is accepted by `Observer` class,
        which calls the underlying C interface
        """
        s = ''
        for key in self.keys():
            s += key
            s += ' : '
            s += self[key]
            s += " "        
        for c in ["u'", 'u"', "{", "}"]:
            s = s.replace(c, '')

        return s

def print_flush(*args):
    """print without newline but with flush"""
    print(*args, end="")
    sys.stdout.flush()

def ascetime(sec):
    """return elapsed time as str.

    Example: return `"0h33:21"` if `sec == 33*60 + 21`. 
    """
    h = sec / 60**2
    m = 60 * (h - h // 1)
    s = 60 * (m - m // 1)
    return "%dh%02d:%02d" % (h, m, s)

class ShortInfo(object):
    def __init__(self):
        self.f_current = None  # function id (not problem id)
        self.d_current = 0  # dimension
        self.t0_dimension = time.time()
        self.evals_dimension = 0
        self.evals_by_dimension = {}
        self.runs_function = 0
    def print(self, problem, end="", **kwargs):
        print(self(problem), end=end, **kwargs)
        sys.stdout.flush()
    def add_evals(self, evals, runs):
        self.evals_dimension += evals
        self.runs_function += runs
    def dimension_done(self):
        self.evals_by_dimension[self.d_current] = (time.time() - self.t0_dimension) / self.evals_dimension
        s = '\n    done in %.1e seconds/evaluation' % (self.evals_by_dimension[self.d_current])
        # print(self.evals_dimension)
        self.evals_dimension = 0
        self.t0_dimension = time.time()
        return s
    def function_done(self):
        s = "(%d)" % self.runs_function + (2 - int(np.log10(self.runs_function))) * ' '
        self.runs_function = 0
        return s
    def __call__(self, problem):
        """uses `problem.id` and `problem.dimension` to decide what to print.
        """
        f = "f" + problem.id.lower().split('_f')[1].split('_')[0]
        res = ""
        if self.f_current and f != self.f_current:
            res += self.function_done() + ' '
        if problem.dimension != self.d_current:
            res += '%s%s, d=%d, running: ' % (self.dimension_done() + "\n\n" if self.d_current else '',
                        ShortInfo.short_time_stap(), problem.dimension)
            self.d_current = problem.dimension
        if f != self.f_current:
            res += '%s' % f
            self.f_current = f
        # print_flush(res)
        return res
    def print_timings(self):
        print("  dimension seconds/evaluations")
        print("  -----------------------------")
        for dim in sorted(self.evals_by_dimension):
            print("    %3d      %.1e " %
                  (dim, self.evals_by_dimension[dim]))
        print("  -----------------------------")
    @staticmethod
    def short_time_stap():
        l = time.asctime().split()
        d = l[0]
        d = l[1] + l[2]
        h, m, s = l[3].split(':')
        return d + ' ' + h + 'h' + m + ':' + s



# ===============================================
# loops over a benchmark problem suite
# ===============================================
def example_experiment(suite, observer, budget,
               max_runs, current_batch, number_of_batches):
    addressed_problems = []
    short_info = ShortInfo()
    for problem_index, problem in enumerate(suite):
        if (problem_index + current_batch - 1) % number_of_batches:
            continue
        observer.observe(problem)
        short_info.print(problem) if verbose else None

        # Begin optimization 
        runs = 0
        while runs < independent_restart:
            p = suite.reset_problem()
            observer.observe(p) 
            short_info.print(p) if verbose else None
            max_evals = budget * p.dimension
            if p.evaluations:
                print('WARNING: %d evaluations were done before the first solver call' %
                p.evaluations)
            user_algorithm(p,p.lower_bounds,p.upper_bounds,max_evals)
            if verbose:
                print_flush(".")
        	if(p.evaluations > max_evals):
        		print("WARNING: something wrong! evaluations times %d is larger than max_evals %d" %
                    p.evaluations, max_evals)
            # End optimization

            short_info.add_evals(p.evaluations, 1)
            p.free()
            runs = runs + 1
        # End optimization
        problem.free()
        addressed_problems += [problem.id]
    print(short_info.function_done() + short_info.dimension_done())
    short_info.print_timings()
    print("  %s done (%d of %d problems benchmarked%s)" %
           (suite_name, len(addressed_problems), len(suite),
             ((" in batch %d of %d" % (current_batch, number_of_batches))
               if number_of_batches > 1 else "")), end="")
    if number_of_batches > 1:
        print("\n    MAKE SURE TO RUN ALL BATCHES", end="")
    return addressed_problems

cfgFile = "configuration.ini"
conf = configuration()
conf.set_cfg(cfgFile)
conf.readcfg()

# ==================================================
# set up: The Setting of Experiments
#         Reading from Configurations by default.
#         Developers can edit it to do test directly.

# ==================================================
suite_name = conf.suite_name 
max_runs = 1e9  # number of (almost) independent trials per problem instance
number_of_batches = 1  # allows to run everything in several batches
current_batch = 1      # 1..number_of_batches
suite_instance = "instances: " + conf.instances_id # "1-100"
suite_options = "dimensions: " + conf.dimensions + " function_indices : " + conf.functions_id
observer_options = ObserverOptions({  # is (inherited from) a dictionary
                    'algorithm_name':'\"'+ conf.algorithm_name + '\"',
                    'result_folder' :  conf.result_folder,
                    'algorithm_info':'\"'+ conf.algorithm_info + '\"',
                    'complete_triggers': conf.complete_triggers,
                    'base_evaluation_triggers' :'\"'+conf.base_evaluation_triggers + '\"',
                    'number_target_triggers' : conf.number_target_triggers,
                    'number_interval_triggers' : conf.number_interval_triggers,
                    'parameters_name' : conf.parameters_name
                    })


# ===============================================
# run (main)
# ===============================================
def main(budget=budget,
         max_runs=max_runs,
         current_batch=current_batch,
         number_of_batches=number_of_batches):

    observer_name = default_observers()[suite_name]
    observer_options.update_gracefully(default_observer_options())
    observer = Observer(observer_name, observer_options.as_string)
    suite = Suite(suite_name, suite_instance, suite_options)
    print("Benchmarking solver '%s' with budget=%d*dimension on %s suite, %s"
          % (conf.algorithm_name, budget,
             suite.name, time.asctime()))
    if number_of_batches > 1:
        print('Batch usecase, make sure you run *all* %d batches.\n' %
              number_of_batches)
    t0 = time.clock()
    example_experiment(suite, observer, budget, max_runs,
               current_batch, number_of_batches)
    print(", %s (%s total elapsed time)." % 
            (time.asctime(), ascetime(time.clock() - t0)))

# ===============================================
if __name__ == '__main__':
    main(budget, max_runs, current_batch, number_of_batches)
