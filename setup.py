import os
import platform
from setuptools import setup, Extension, find_packages

with open("README.md", "r") as fh:
    long_description = fh.read()


if platform.system() == "Darwin":
    os.environ["CC"] = "clang"
    os.environ["CXX"] = "clang"



has_wrap_file = os.path.isfile(os.path.join("src", "IOHprofiler_wrap.cpp"))
sources = [
     os.path.join("src", x)
     for x in os.listdir("src") if x.endswith(".cpp") or (
        x.endswith(".i") and not has_wrap_file)
]

iohprofiler = Extension('IOHexperimenter._IOHprofiler', 
    swig_opts=(None if has_wrap_file else 
        ["-c++", "-outdir", "IOHexperimenter"]), 
    sources=sources
)


setup(
    name="IOHexperimenter",
    version="0.2.7",
    author="Furong Ye, Diederick Vermetten, and Hao Wang",
    author_email="f.ye@liacs.leidenuniv.nl",
    packages=find_packages(),
    description="The experimenter for Iterative Optimization Heuristic",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/IOHprofiler/IOHexperimenter",
    ext_modules = [iohprofiler],
    package_dir={'IOHexperimenter': 'IOHexperimenter'},
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)"
    ],
    python_requires='>=3.6',
    install_requires=[ 
        "numpy",        
    ]
)
