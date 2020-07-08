import ioh

class MyProblem(ioh.Problem):
    def __init__(self, dim=4):
        self.instance = 1
        self.dimension = dim

    def call(self, *args ):
        sum = 0
        for x in args:
            sum += x*x
        return sum

    def get_instance(self):
        return self.instance

    def get_number_of_variables(self):
        return self.dimension

    def get_lowerbound(self):
        return [-5] * self.dimension

    def get_upperbound(self):
        return [5] * self.dimension

    def get_minimization(self):
        return True

    def get_problem_id(self):
        return 1

    def get_name(self):
        return "First_test"

    def get_type(self):
        return "Test_benchmark"

    def get_number_of_objectives(self):
        return 1


pb1 = MyProblem()

