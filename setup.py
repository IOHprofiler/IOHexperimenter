import setuptools, sys, os, sysconfig, glob
from shutil import copyfile
from setuptools.command.install import install

class CustomInstall(install):
    def run(self):
        include_path = sysconfig.get_config_var('INCLUDEDIR')
        header = glob.glob(os.path.join(include_path, '*/Python.h'), recursive=True)[0]
        include_path = os.path.dirname(header)

        lib_path = sysconfig.get_config_var('LIBDIR')
        lib_file = glob.glob(os.path.join(lib_path, './*.so')) + \
              glob.glob(os.path.join(lib_path,'./*.dylib'))

        if len(lib_file) != 0:
            lib_file = lib_file[0]
        else:
            raise Exception('Python dynamic library is not found...')

        command = 'sed -e "s|^PYTHON_LIB=$|PYTHON_LIB={0}|g"\
            -e "s|-I/python-header|-I{1}|g" Makefile.in > Makefile'.format(lib_file, include_path)

        os.system(command)
        os.system('make')
        
        copyfile('_IOHprofiler.so', 'IOHexperimenter/_IOHprofiler.so')
        copyfile('IOHprofiler.py', 'IOHexperimenter/IOHprofiler.py')

        install.run(self)

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    cmdclass={'install': CustomInstall},
    name="IOHexperimenter",
    version="0.0.1.dev3",
    author="Furong Ye, Diederick Vermetten, and Hao Wang",
    author_email="f.ye@liacs.leidenuniv.nl",
    description="The experimenter for Iterative Optimization Heuristic",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/IOHprofiler/IOHexperimenter",
    packages=setuptools.find_packages(),
    package_dir={'': '.'},
    package_data={'': ['_IOHprofiler.so']},
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)"
    ],
    python_requires='>=3.6'
)

# os.remove('IOHexperimenter/_IOHprofiler.so')
