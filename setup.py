import os
from setuptools import setup, Extension, find_packages

with open("README.md", "r") as fh:
    long_description = fh.read()

setup(
    name="IOHexperimenter",
    version="0.2.5",
    author="Furong Ye, Diederick Vermetten, and Hao Wang",
    author_email="f.ye@liacs.leidenuniv.nl",
    packages=find_packages(),
    description="The experimenter for Iterative Optimization Heuristic",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/IOHprofiler/IOHexperimenter",
    ext_modules = [
        Extension('_IOHprofiler', sources=[
            os.path.realpath(os.path.join("src", x))
            for x in os.listdir("src") if x.endswith("cpp")
        ])
    ],
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