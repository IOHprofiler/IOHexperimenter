#!/usr/bin/env python
## -*- mode: python -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import sys
import os
from os.path import join
import shutil
import tempfile
import subprocess
from subprocess import STDOUT
import platform
import time
import glob


## Change to the root directory of repository and add our tools/
## subdirectory to system wide search path for modules.
os.chdir(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.abspath(join('code-experiments', 'tools')))

from amalgamate import amalgamate
from cocoutils import make, run, python, check_output
from cocoutils import copy_file, expand_file, write_file
from cocoutils import git_version, git_revision

CORE_FILES = ['code-experiments/src/IOHprofiler_random.c',
              'code-experiments/src/IOHprofiler_suite.c',
              'code-experiments/src/IOHprofiler_observer.c',
             ]

_verbosity = False

################################################################################
## C
def build_c():
    """ Builds the C source code """
    global RELEASE
    amalgamate(CORE_FILES + ['code-experiments/src/IOHprofiler_runtime_c.c'],
               'code-experiments/build/c/IOHprofiler.c', RELEASE,
               {"IOHprofiler_VERSION": git_version(pep440=True)})
    expand_file('code-experiments/src/IOHprofiler.h', 'code-experiments/build/c/IOHprofiler.h',
                {"IOHprofiler_VERSION": git_version(pep440=True)})
    write_file(git_revision(), "code-experiments/build/c/REVISION")
    write_file(git_version(), "code-experiments/build/c/VERSION")
    if 11 < 3:
        python('code-experiments/build/c', ['make.py', 'clean'], verbose=_verbosity)
        python('code-experiments/build/c', ['make.py', 'all'], verbose=_verbosity)
    else:
        make("code-experiments/build/c", "clean", verbose=_verbosity)
        make("code-experiments/build/c", "all", verbose=_verbosity)


def run_c_example():
    """ Builds and runs the example experiment in C """
    build_c()
    try:
        run('code-experiments/build/c', ['./example_experiment'], verbose=_verbosity)
    except subprocess.CalledProcessError:
        sys.exit(-1)

def run_c():
    """ Builds and runs the example experiment in C """
    build_c()
    try:
        run('code-experiments/build/c', ['./user_experiment'], verbose=_verbosity)
    except subprocess.CalledProcessError:
        sys.exit(-1)


################################################################################
## Python 2
def _prep_python():
    global RELEASE
    amalgamate(CORE_FILES + ['code-experiments/src/IOHprofiler_runtime_c.c'],
               'code-experiments/build/python/cython/IOHprofiler.c',
               RELEASE, {"IOHprofiler_VERSION": git_version(pep440=True)})
    expand_file('code-experiments/src/IOHprofiler.h',
                'code-experiments/build/python/cython/IOHprofiler.h',
                {'IOHprofiler_VERSION': git_version(pep440=True)})
    copy_file('code-experiments/build/python/README.md',
              'code-experiments/build/python/README.txt')
    expand_file('code-experiments/build/python/setup.py.in',
                'code-experiments/build/python/setup.py',
                {'IOHprofiler_VERSION': git_version(pep440=True)})  # hg_version()})
    # if 'darwin' in sys.platform:  # a hack to force cythoning
    #     run('code-experiments/build/python/cython', ['cython', 'interface.pyx'])


def build_python():
    _prep_python()
    ## Force distutils to use Cython
    # os.environ['USE_CYTHON'] = 'true'
    # python('code-experiments/build/python', ['setup.py', 'sdist'])
    # python(join('code-experiments', 'build', 'python'), ['setup.py', 'install', '--user'])
    python(join('code-experiments', 'build', 'python'), ['setup.py', 'install', '--user'])
    # os.environ.pop('USE_CYTHON')


def run_python_example():
    """ Builds and installs the Python module `IOHprofiler_python` and runs the
    `example_experiment.py` as a simple test case. """
    build_python()
    try:
        python(os.path.join('code-experiments', 'build', 'python'),
               ['example_experiment.py'])
    except subprocess.CalledProcessError:
        sys.exit(-1)

def run_python():
    """ Builds and installs the Python module `IOHprofiler_python` and runs the
    `example_experiment.py` as a simple test case. """
    build_python()
    try:
        python(os.path.join('code-experiments', 'build', 'python'),
               ['user_experiment.py'])
    except subprocess.CalledProcessError:
        print("error")
        sys.exit(-1)


################################################################################
## Global
def build():
    builders = [
        build_c,
        build_python,
    ]
    for builder in builders:
        try:
            builder()
        except:
            failed = str(builder)
            print("============")
            print('   ERROR: %s failed, call "./do.py %s" individually'
                  % (failed, failed[failed.find('build_'):].split()[0]) +
                  ' for a more detailed error report')
            print("============")


def run_all():
    run_c()
    run_python()



def verbose(args):
    global _verbosity
    _verbosity = True
    main(args)
    _verbosity = False

def help():
    print("""IOHprofiler framework bootstrap tool. Version %s

Usage: do.py <command> <arguments>


Available commands for users:

  build-c                 - Build C module
  build-python            - Build Python modules

  run-c                   - Build and run example experiment in C
  run-python              - Build and run example experiment in Python

Available commands for developers:

  build                   - Build C and Python modules
  run                     - Run example experiments in C and Python

""" % git_version(pep440=True))


def main(args):
    if len(args) < 1:
        help()
        sys.exit(0)
    cmd = args[0].replace('_', '-').lower()
    if cmd == 'build': build()
    elif cmd == 'run': run_all()
    elif cmd == 'build-c': build_c()
    elif cmd == 'build-python': build_python()
    elif cmd == 'run-c': run_c()
    elif cmd == 'run-c-example': run_c_example()
    elif cmd == 'run-python': run_python()
    elif cmd == 'run-python-example': run_python_example()
    else: help()


if __name__ == '__main__':
    RELEASE = os.getenv('IOHprofiler_RELEASE', 'false') == 'true'
    main(sys.argv[1:])
