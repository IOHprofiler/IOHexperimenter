from ioh import get_problem, ProblemClass

print('DynamicBinValUniform ===============================================================')

print('instance 1 ===============================================================')

f1 = get_problem(10001, 1, 5, ProblemClass.INTEGER)
x1 = [0, 1, 1, 1, 0]
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

print('instance 42 ===============================================================')

f1 = get_problem(10001, 42, 5, ProblemClass.INTEGER)
x1 = [0, 1, 1, 1, 0]
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

print('instance 1 ===============================================================')

f1 = get_problem(10001, 1, 5, ProblemClass.INTEGER)
x1 = [0, 1, 1, 1, 0]
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)

print('instance 13 ===============================================================')

dynamic_bin_val = get_problem(10001, 1, 5, ProblemClass.INTEGER)
print("DynamicBinValUniform [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValUniform [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValUniform [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValUniform [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValUniform [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))

# dynamic_bin_val.step()
print("DynamicBinValUniform [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValUniform [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValUniform [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValUniform [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValUniform [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))

dynamic_bin_val.step()
print("DynamicBinValUniform [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValUniform [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValUniform [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValUniform [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValUniform [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))

dynamic_bin_val.step()
print("DynamicBinValUniform [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValUniform [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValUniform [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValUniform [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValUniform [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))

dynamic_bin_val.step()
print("DynamicBinValUniform [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValUniform [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValUniform [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValUniform [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValUniform [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))












print('DynamicBinValPowersOfTwo ===============================================================')

print('instance 1 ===============================================================')

f1 = get_problem(10002, 1, 5, ProblemClass.INTEGER)
x1 = [0, 1, 1, 1, 0]
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

print('instance 42 ===============================================================')

f1 = get_problem(10002, 42, 5, ProblemClass.INTEGER)
x1 = [0, 1, 1, 1, 0]
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

print('instance 1 ===============================================================')

f1 = get_problem(10002, 1, 5, ProblemClass.INTEGER)
x1 = [0, 1, 1, 1, 0]
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)

print('instance 13 ===============================================================')

dynamic_bin_val = get_problem(10002, 1, 5, ProblemClass.INTEGER)
print("DynamicBinValPowersOfTwo [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))

# dynamic_bin_val.step()
print("DynamicBinValPowersOfTwo [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))

dynamic_bin_val.step()
print("DynamicBinValPowersOfTwo [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))

dynamic_bin_val.step()
print("DynamicBinValPowersOfTwo [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))

dynamic_bin_val.step()
print("DynamicBinValPowersOfTwo [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValPowersOfTwo [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))










print('DynamicBinValPareto ===============================================================')

print('instance 1 ===============================================================')

f1 = get_problem(10003, 1, 5, ProblemClass.INTEGER)
x1 = [0, 1, 1, 1, 0]
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

print('instance 42 ===============================================================')

f1 = get_problem(10003, 42, 5, ProblemClass.INTEGER)
x1 = [0, 1, 1, 1, 0]
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

print('instance 1 ===============================================================')

f1 = get_problem(10003, 1, 5, ProblemClass.INTEGER)
x1 = [0, 1, 1, 1, 0]
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)
print(y1)

y1 = f1(x1)
print()
print(x1)
print(y1)

f1.step()
y1 = f1(x1)
print()
print(x1)

print('instance 13 ===============================================================')

dynamic_bin_val = get_problem(10003, 1, 5, ProblemClass.INTEGER)
print("DynamicBinValPareto [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValPareto [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValPareto [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValPareto [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValPareto [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))

# dynamic_bin_val.step()
print("DynamicBinValPareto [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValPareto [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValPareto [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValPareto [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValPareto [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))

dynamic_bin_val.step()
print("DynamicBinValPareto [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValPareto [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValPareto [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValPareto [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValPareto [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))

dynamic_bin_val.step()
print("DynamicBinValPareto [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValPareto [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValPareto [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValPareto [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValPareto [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))

dynamic_bin_val.step()
print("DynamicBinValPareto [1, 0, 0, 0, 0]:", dynamic_bin_val([1, 0, 0, 0, 0]))
print("DynamicBinValPareto [0, 1, 0, 0, 0]:", dynamic_bin_val([0, 1, 0, 0, 0]))
print("DynamicBinValPareto [0, 0, 1, 0, 0]:", dynamic_bin_val([0, 0, 1, 0, 0]))
print("DynamicBinValPareto [0, 0, 0, 1, 0]:", dynamic_bin_val([0, 0, 0, 1, 0]))
print("DynamicBinValPareto [0, 0, 0, 0, 1]:", dynamic_bin_val([0, 0, 0, 0, 1]))
