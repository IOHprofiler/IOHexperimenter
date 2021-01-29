import os
import platform
from setuptools import setup, Extension, find_packages

with open("README.md", "r") as fh:
    long_description = fh.read()


if platform.system() == "Darwin":
    os.environ["CC"] = "clang"
    os.environ["CXX"] = "clang"
    os.environ["ARCHFLAGS"] = "-std=c++14"

extra_compile_args = None if platform.system() == 'Windows' else [
    "-std=c++14"
]

has_wrap_file = os.path.isfile(os.path.join("src", "IOHprofiler_wrap.cpp"))
sources = [
     os.path.join("src", x)
     for x in os.listdir("src") 
     if x.endswith(".cpp") or (x.endswith(".i") and not has_wrap_file)
]

iohprofiler = Extension('IOHexperimenter._IOHprofiler', 
    swig_opts=(None if has_wrap_file else 
        ["-c++", "-outdir", "IOHexperimenter"]), 
    sources=sources,
    include_dirs=["src"],
    extra_compile_args=extra_compile_args
)

__version__ = "auto"
gh_ref = os.environ.get("GITHUB_REF")
if gh_ref:
    *_, tag = gh_ref.split("/")
    __version__ = tag.replace("v", "")
    


setup(
    name="IOHexperimenter",
    version=__version__,
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
