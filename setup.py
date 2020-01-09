import setuptools, sys, os
from shutil import copyfile
from distutils.util import get_platform

try:
    from wheel.bdist_wheel import bdist_wheel as _bdist_wheel
    class bdist_wheel(_bdist_wheel):
        def finalize_options(self):
            _bdist_wheel.finalize_options(self)
            self.universal = False
            self.plat_name_supplied = True
            self.plat_name = get_platform()

except ImportError:
    bdist_wheel = None

lib_file = 'lib/_IOHprofiler'
if sys.version.startswith('3.7'):
    lib_file += '-py37'
elif sys.version.startswith('3.6'):
    lib_file += '-py36'

if get_platform().startswith('macosx'):
    lib_file += '-macosx_x86_64.so' 
elif get_platform().startswith('linux'):
    lib_file += '-linux_x86_64.so'

copyfile(lib_file, 'IOHexperimenter/_IOHprofiler.so')

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    cmdclass={'bdist_wheel': bdist_wheel},
    name="IOHexperimenter",
    version="0.0.1.dev1",
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

os.remove('IOHexperimenter/_IOHprofiler.so')
