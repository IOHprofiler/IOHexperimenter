from collections.abc import Set

import ioh.iohcpp


class Trigger:
    """Base class for all Triggers"""

    def __call__(self, arg0: ioh.iohcpp.LogInfo, arg1: ioh.iohcpp.MetaData, /) -> bool: ...

    def reset(self) -> None: ...

class Always(Trigger):
    """Trigger that always evaluates to true"""

    def __init__(self) -> None: ...

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

ALWAYS: Always = ...

class OnImprovement(Trigger):
    """
    Trigger that evaluates to true when improvement of the objective function is observed
    """

    def __init__(self) -> None: ...

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

ON_IMPROVEMENT: OnImprovement = ...

class OnDeltaImprovement(Trigger):
    """
    Trigger that evaluates to true when improvement of the objective function is observed of at least greater than delta. It keeps the best value of a run, so loggers used at the same time (e.g. in different threads) each need their own, rather than sharing ON_DELTA_IMPROVEMENT.
    """

    def __init__(self, delta: float = 1e-10) -> None: ...

    @property
    def delta(self) -> float: ...

    @delta.setter
    def delta(self, arg: float, /) -> None: ...

    @property
    def best_so_far(self) -> float: ...

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

ON_DELTA_IMPROVEMENT: OnDeltaImprovement = ...

class OnViolation(Trigger):
    """
    Trigger that evaluates to true when there is a contraint violation. It counts the violations, so loggers used at the same time (e.g. in different threads) each need their own, rather than sharing ON_VIOLATION.
    """

    def __init__(self) -> None: ...

    @property
    def violations(self) -> int: ...

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

ON_VIOLATION: OnViolation = ...

class At(Trigger):
    def __init__(self, time_points: Set[int]) -> None:
        """
        Trigger that evaluates to true at specific time points

        Parameters
        ----------
        time_points: list[int]
            The time points at which to trigger
        """

    @property
    def time_points(self) -> set[int]: ...

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

class Each(Trigger):
    def __init__(self, interval: int, starting_at: int = 0) -> None:
        """
        Trigger that evaluates to true at a given interval

        Parameters
        ----------
        interval: int
            The interval at which to trigger
        starting_at: int
            The starting time of the interval, defaults to 0.
        """

    @property
    def interval(self) -> int: ...

    @property
    def starting_at(self) -> int: ...

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...

class During(Trigger):
    def __init__(self, time_ranges: Set[tuple[int, int]]) -> None:
        """
        Trigger that evaluates to true during a given interval.

        Parameters
        ----------
        time_ranges: set[tuple[int, int]]
            The ranges/intervals at which the log the data.
        """

    @property
    def time_ranges(self) -> set[tuple[int, int]]: ...

    def __getstate__(self) -> tuple: ...

    def __setstate__(self, arg: tuple, /) -> None: ...
