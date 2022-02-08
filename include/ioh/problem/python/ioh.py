
import abc

class Problem(metaclass=abc.ABCMeta):

    @abc.abstractmethod
    def call(self, *args ):
        "Objective function"
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `call(self,*args):`')

    
    @abc.abstractmethod
    def get_instance(self):
        "i.e. for invariance checking"
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `get_instance(self)`')

    @abc.abstractmethod
    def get_number_of_variables(self):
        "Dimension"
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `get_number_of_variables(self)`')

    @abc.abstractmethod
    def get_lowerbound(self):
        "Bounds"
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `get_lowerbound(self)`')

    @abc.abstractmethod
    def get_upperbound(self):
        "Bounds"
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

    @abc.abstractmethod
    def get_number_of_objectives(self):
        raise NotImplementedError('Classes inheriting from ioh.Problem must define `get_number_of_objectives(self)`')

