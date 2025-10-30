import ioh
import numpy as np

class RandomSearch:
    'Simple random search algorithm'
    def __init__(self, n: int, length: float = 0.0):
        self.n: int = n
        self.length: float = length
        
    def __call__(self, problem: ioh.problem.RealSingleObjective) -> None:
        'Evaluate the problem n times with a randomly generated solution'
        
        for _ in range(self.n):
            # We can use the problems bounds accessor to get information about the problem bounds
            x = np.random.uniform(problem.bounds.lb, problem.bounds.ub)
            self.length = np.linalg.norm(x)
            
            problem(x)             
budget = 1000
my_algorithm = RandomSearch(n=budget)
exp = ioh.Experiment(
    my_algorithm,
    fids=[1, 2, 3],
    iids=[1],
    dims=[5, 10],
    reps=5,
)
exp()  # run the benchmark
exp.publish(
    zenodo_token="<your token here>",
    title="Benchmarking IOH publish example",
    description="Log data for the benchmark run.",
    creators=[{"name": "van Stein, Niki"}],
    budget=budget,
    keywords=["optimization", "benchmark", "ioh"],
    sandbox=True,
)