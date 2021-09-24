# -*- coding: utf-8 -*-
import os
import sys
import shutil
import platform
import subprocess

from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext

# Convert distutils Windows platform specifiers to CMake -A arguments
PLAT_TO_CMAKE = {
    "win32": "Win32",
    "win-amd64": "x64",
    "win-arm32": "ARM",
    "win-arm64": "ARM64",
}

if platform.system() == "Darwin":
    os.environ["CC"] = "clang"
    os.environ["CXX"] = "clang"
    os.environ["ARCHFLAGS"] = "-std=c++14"


# A CMakeExtension needs a sourcedir instead of a file list.
# The name must be the _single_ output extension from the CMake build.
# If you need multiple extensions, see scikit-build.
class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
    def generate_stubs(self):
        ext, *_ = self.extensions

        # remove any existing stubs
        for stubfile in os.listdir(os.path.join(ext.sourcedir, "ioh")):
            stubfile = os.path.join(ext.sourcedir, "ioh", stubfile)
            if stubfile.endswith(".pyi"):
                os.remove(stubfile)
        
        for subdir in ("iohcpp", "logger"):
            if os.path.isdir(os.path.join(ext.sourcedir, "ioh", subdir)):
                shutil.rmtree(os.path.join(ext.sourcedir, "ioh", subdir))

        # generate stub files
        command = """stubgen -m ioh -m ioh.iohcpp \
                -m ioh.iohcpp.problem \
                -m ioh.iohcpp.logger \
                -m ioh.iohcpp.suite \
                -m ioh.iohcpp.logger.trigger \
                -m ioh.iohcpp.logger.property \
                -o ./"""
        subprocess.check_call(command, cwd=ext.sourcedir, shell=True)

    def run(self):
        super().run()
        try:
            self.generate_stubs()
        except subprocess.CalledProcessError: 
            pass

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))

        # required for auto-detection of auxiliary "native" libs
        if not extdir.endswith(os.path.sep):
            extdir += os.path.sep

        cfg = "Debug" if self.debug else "Release"
        print("build-type:", cfg)

        # CMake lets you override the generator - we need to check this.
        # Can be set with Conda-Build, for example.
        cmake_generator = os.environ.get("CMAKE_GENERATOR", "")

        # Set Python_EXECUTABLE instead if you use PYBIND11_FINDPYTHON
        # EXAMPLE_VERSION_INFO shows you how to pass a value into the C++ code
        # from Python.
        cmake_args = [
            "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={}".format(extdir),
            "-DPYTHON_EXECUTABLE={}".format(sys.executable),
            "-DBUILD_PYTHON_PACKAGE=ON",
            "-DBUILD_TESTS=OFF",
            "-DBUILD_EXAMPLE=OFF",
            "-DEXAMPLE_VERSION_INFO={}".format(self.distribution.get_version()),
            "-DCMAKE_BUILD_TYPE={}".format(cfg),  # not used on MSVC, but no harm
        ]
        build_args = []

        if self.compiler.compiler_type != "msvc":
            # Using Ninja-build since it a) is available as a wheel and b)
            # multithreads automatically. MSVC would require all variables be
            # exported for Ninja to pick it up, which is a little tricky to do.
            # Users can override the generator with CMAKE_GENERATOR in CMake
            # 3.15+.
            if not cmake_generator:
                cmake_args += ["-GNinja"]

        else:

            # Single config generators are handled "normally"
            single_config = any(x in cmake_generator for x in {"NMake", "Ninja"})

            # CMake allows an arch-in-generator style for backward compatibility
            contains_arch = any(x in cmake_generator for x in {"ARM", "Win64"})

            # Specify the arch if using MSVC generator, but only if it doesn't
            # contain a backward-compatibility arch spec already in the
            # generator name.
            if not single_config and not contains_arch:
                cmake_args += ["-A", PLAT_TO_CMAKE[self.plat_name]]

            # Multi-config generators have a different way to specify configs
            if not single_config:
                cmake_args += [
                    "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}".format(cfg.upper(), extdir)
                ]
                build_args += ["--config", cfg]

        # Set CMAKE_BUILD_PARALLEL_LEVEL to control the parallel build level
        # across all generators.
        if "CMAKE_BUILD_PARALLEL_LEVEL" not in os.environ:
            # self.parallel is a Python 3 only way to set parallel jobs by hand
            # using -j in the build_ext call, not supported by pip or PyPA-build.
            if hasattr(self, "parallel") and self.parallel:
                # CMake 3.12+ only.
                build_args += ["-j{}".format(self.parallel)]

        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        subprocess.check_call(
            ["cmake", ext.sourcedir] + cmake_args, cwd=self.build_temp
        )
        subprocess.check_call(
            ["cmake", "--build", "."] + build_args, cwd=self.build_temp
        )


# The information here can also be placed in setup.cfg - better separation of
# logic and declaration, and simpler if you include description/version in a file.
__version__ = "2.0.0.0"
__version__ = "0.2.9.9"
gh_ref = os.environ.get("GITHUB_REF")
if gh_ref:
    *_, tag = gh_ref.split("/")
    __version__ = tag.replace("v", "")

with open("README.md", "r") as fh:
    long_description = fh.read()

iohcpp = CMakeExtension("ioh.iohcpp")

setup(
    name="ioh",
    version=__version__,
    author="Jacob de Nobel, Furong Ye, Diederick Vermetten, Hao Wang, Carola Doerr and Thomas Bäck",
    author_email="iohprofiler@liacs.leidenuniv.nl",
    description="The experimenter for Iterative Optimization Heuristics",
    long_description=long_description,
    long_description_content_type="text/markdown",
    packages=find_packages(),
    package_dir={'IOHexperimenter':'ioh'},
    package_data={"ioh": [
        "ioh/__init__.pyi",
        "ioh/iohcpp/__init__.pyi",
        "ioh/iohcpp/problem.pyi",
        "ioh/iohcpp/suite.pyi",
        "ioh/iohcpp/logger/__init__.pyi",
        "ioh/iohcpp/logger/property.pyi",
        "ioh/iohcpp/logger/trigger.pyi"
    ]},
    ext_modules=[iohcpp],
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False,
    test_suite='tests.python',
    python_requires='>=3.6',
    setup_requires=['cmake', 'ninja', 'pybind11', 'mypy']
)
