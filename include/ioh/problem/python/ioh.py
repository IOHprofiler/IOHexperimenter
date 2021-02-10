
import abc

class Problem(metaclass=abc.ABCMeta):

    # Objective function
    @abc.abstractmethod
    def call(self, *args ):
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `call(self,*args):`')

    # i.e. for invariance checking
    @abc.abstractmethod
    def get_instance(self):
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `get_instance(self)`')

    # Dimension
    @abc.abstractmethod
    def get_number_of_variables(self):
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `get_number_of_variables(self)`')

    # Bounds
    @abc.abstractmethod
    def get_lowerbound(self):
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `get_lowerbound(self)`')

    @abc.abstractmethod
    def get_upperbound(self):
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `et_upperbound(self)`')


    # Generic information
    @abc.abstractmethod
    def get_minimization(self):
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `get_minimization(self)`')

    @abc.abstractmethod
    def get_problem_id(self):
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `get_problem_id(self)`')

    @abc.abstractmethod
    def get_name(self):
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `get_name(self)`')

    @abc.abstractmethod
    def get_type(self):
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `get_type(self)`')
        return "Test_benchmark"

    @abc.abstractmethod
    def get_number_of_objectives(self):
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `get_number_of_objectives(self)`')

