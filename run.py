import ioh


def f(x):
    return 0

if __name__ == '__main__':
    p = ioh.problem.wrap_real_problem(f, "CustomProblem")

    p = ioh.problem.Real.create("CustomProblem", 2, 4)
    p1 = ioh.problem.Real.create("CustomProblem", 2, 2)
    p2 = ioh.problem.Real.create("CustomProblem", 2, 3)
    p3 = ioh.problem.Real.create("CustomProblem", 2, 5)
    
    print(p([0]*4))
    print(p1([0]*2))
    print(p3([0]*5))
