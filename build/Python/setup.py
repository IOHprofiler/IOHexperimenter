#!/usr/bin/env python

"""
setup.py file for SWIG C\+\+/Python IOHprofiler
"""
from distutils.core import setup, Extension

IOHprofiler_module = Extension('_IOHprofiler',
    sources=['src/coco_transformation.cpp','src/IOHprofiler_common.cpp','src/IOHprofiler_csv_logger.cpp','IOHprofiler_wrap.cxx'],
    include_dirs = ['./src'],
    extra_compile_args=['-std=c++11','-fPIC'],
    extra_link_args=['-std=c++11','-fPIC'],
)
setup (
    name = 'IOHprofiler',
    version = '0.1',
    author = "Furong Ye",
    description = """Simple swig C\+\+/Python example""",
    ext_modules = [IOHprofiler_module],
    py_modules = ["IOHprofiler"],
)
