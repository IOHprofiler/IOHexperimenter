import ioh.iohcpp


class AbstractProperty:
    """Base class for all Properties"""

    def __init__(self, arg: str, /) -> None: ...

    def __call__(self, arg: ioh.iohcpp.LogInfo, /) -> float | None: ...

    @property
    def name(self) -> str: ...

    @name.setter
    def name(self, arg: str, /) -> None: ...

    def call_to_string(self, arg0: ioh.iohcpp.LogInfo, arg1: str, /) -> str: ...

class Property(AbstractProperty):
    def __init__(self, container: object, attribute: str) -> None:
        """
        Wrapper for properties with a Python container

        Parameters
        ----------
        container: object
            The object which contains the property to be logged
        attribute: str
            The name of the property to be logger, must be an attribute on container
        """

    def call_to_string(self, arg0: ioh.iohcpp.LogInfo, arg1: str, /) -> str: ...

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

class Evaluations(AbstractProperty):
    def __init__(self, name: str, format: str) -> None:
        """Property which tracks the Evaluations"""

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

EVALUATIONS: Evaluations = ...

class RawY(AbstractProperty):
    def __init__(self, name: str, format: str) -> None:
        """Property which tracks the RawY"""

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

RAWY: RawY = ...

class RawYBest(AbstractProperty):
    def __init__(self, name: str, format: str) -> None:
        """Property which tracks the RawYBest"""

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

RAWYBEST: RawYBest = ...

class TransformedY(AbstractProperty):
    def __init__(self, name: str, format: str) -> None:
        """Property which tracks the TransformedY"""

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

TRANSFORMEDY: TransformedY = ...

class TransformedYBest(AbstractProperty):
    def __init__(self, name: str, format: str) -> None:
        """Property which tracks the TransformedYBest"""

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

TRANSFORMEDYBEST: TransformedYBest = ...

class CurrentY(AbstractProperty):
    def __init__(self, name: str, format: str) -> None:
        """Property which tracks the CurrentY"""

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

CURRENTY: CurrentY = ...

class CurrentBestY(AbstractProperty):
    def __init__(self, name: str, format: str) -> None:
        """Property which tracks the CurrentBestY"""

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

CURRENTBESTY: CurrentBestY = ...

class Violation(AbstractProperty):
    def __init__(self, name: str, format: str) -> None:
        """Property which tracks the Violation"""

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

VIOLATION: Violation = ...

class Penalty(AbstractProperty):
    def __init__(self, name: str, format: str) -> None:
        """Property which tracks the Penalty"""

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

PENALTY: Penalty = ...
