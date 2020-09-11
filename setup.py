import os
from setuptools import setup, Extension, find_packages

with open("README.md", "r") as fh:
    long_description = fh.read()


wrap_file = os.path.join("src", "IOHprofiler_wrap.cpp")
if os.path.isfile(wrap_file):
    os.remove(wrap_file)


iohprofiler = Extension('_IOHprofiler', 
    swig_opts=["-c++", "-outdir", "IOHexperimenter"], 
    sources=[
        os.path.join("src", x)
        for x in os.listdir("src") if x.endswith(".cpp") or x.endswith(".i")
    ]
)


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