import setuptools, sys, os, sysconfig, glob
from pathlib import Path
from shutil import move
from distutils.command.build import build

class CustomBuild(build):
    def run(self):
        include_path = sysconfig.get_config_var('INCLUDEDIR')
        py_version = sysconfig.get_python_version()
        header = [str(p) for p in Path(include_path).rglob('python' + py_version + '*/Python.h')]

        if len(header) == 0:
            raise Exception('Python.h not found...')
        header = header[0]
        include_path = os.path.dirname(header)

        lib_path = Path(sysconfig.get_config_var('LIBDIR'))
        py_lib = 'libpython' + py_version + '*'
        lib_file = [str(p) for p in lib_path.rglob(py_lib + '.so')] + \
            [str(p) for p in lib_path.rglob(py_lib + '.dylib')] + \
            [str(p) for p in lib_path.rglob(py_lib + '.a')]

        if len(lib_file) != 0:
            lib_file = lib_file[0]
        else:
            raise Exception('Python dynamic library not found...')

        command = 'sed -e "s|py_lib=|py_lib={0}|g"\
            -e "s|-I/python-header|-I{1}|g" Makefile.in > Makefile'.format(lib_file, include_path)

        os.system(command)
        os.system('make')
        
        move('_IOHprofiler.so', 'IOHexperimenter/_IOHprofiler.so')
        move('IOHprofiler.py', 'IOHexperimenter/IOHprofiler.py')

        super().run()

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    cmdclass={
        'build': CustomBuild,
    },
    name="IOHexperimenter",
    version="0.1.0",
    author="Furong Ye, Diederick Vermetten, and Hao Wang",
    author_email="f.ye@liacs.leidenuniv.nl",
    description="The experimenter for Iterative Optimization Heuristic",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/IOHprofiler/IOHexperimenter",
    packages=setuptools.find_packages(),
    package_dir={'IOHexperimenter': 'IOHexperimenter'},
    package_data={'IOHexperimenter': ['_IOHprofiler.so']},
    include_package_data=True,
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)"
    ],
    python_requires='>=3.6',
    zip_safe=False,  # if enabled, *.so files would be be compressed when buiding the .egg archive
)