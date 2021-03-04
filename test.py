from ioh import problem


f1 = problem.RealFactory.instance()
f2 = problem.IntegerFactory.instance()
for f in (f1, f2):
    for i in sorted(f.ids()):
        p = f.create(i, 1, 4)
        print(p, p([1, 1, 1, 1])[0])