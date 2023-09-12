from ioh import get_problem, ProblemClass

f1 = get_problem(10001, 1, 5, ProblemClass.INTEGER)
x1 = [0, 1, 1, 1, 0]
y1 = f1(x1)
print()
print("function 10001")
print(x1)
print(y1)
