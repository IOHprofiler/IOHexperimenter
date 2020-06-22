#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Config file, containing all hardcoded values such as parallelization settings
"""

from multiprocessing import cpu_count

### Customizable settings for 
Pool_max_threads = 12
use_pebble = True
evaluate_parallel = True

num_threads = 1  # Default case, always true
try:
    num_threads = min(cpu_count(), Pool_max_threads)
    if num_threads > 1:
        allow_parallel = True
    else:
        allow_parallel = False
except NotImplementedError:
    allow_parallel = False

### Parallelization Settings ###
use_MPI = False
MPI_num_host_threads = 16
MPI_num_hosts = 12
MPI_num_total_threads = MPI_num_host_threads * MPI_num_hosts