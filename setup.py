import setuptools, sys, os, sysconfig, glob, subprocess, platform, re
from pathlib import Path
from shutil import move, copy
from distutils.command.build import build
from wheel.bdist_wheel import bdist_wheel as _bdist_wheel
from pdb import set_trace

_platform = platform.system()

def get_py_info():
    py_version = sysconfig.get_config_var('py_version')
    if _platform == 'Windows':
        prefix = sysconfig.get_config_var('prefix')
        include_path = os.path.join(prefix, 'include')
        lib_path = os.path.join(prefix, 'libs')
        set_trace()
        lib_file = [str(p) for p in Path(lib_path).rglob('python*' + '.lib')]
        header = [str(p) for p in Path(include_path).rglob('Python.h')]
    else: 
        include_path = sysconfig.get_config_var('INCLUDEDIR')
        header = [
            str(p) for p in Path(include_path).rglob(
                'python' + py_version + '*/Python.h'
            )
        ]
        lib_path = Path(sysconfig.get_config_var('LIBDIR'))
        py_lib = 'libpython' + py_version + '*'
        lib_file = [str(p) for p in lib_path.rglob(py_lib + '.so')] + \
            [str(p) for p in lib_path.rglob(py_lib + '.dylib')] + \
            [str(p) for p in lib_path.rglob(py_lib + '.a')]
    
    if len(header) != 0:
        header = os.path.realpath(header[0])
    else:
        raise Exception('Python.h not found...')

    if len(lib_file) != 0:
        lib_file = os.path.realpath(lib_file[0])
    else:
        raise Exception('Python dynamic library not found...')

    include_path = os.path.dirname(header)
    if _platform == 'Windows':
        lib_file = lib_file.replace("\\", "\\\\")
        include_path = include_path.replace("\\", "\\\\")

    return {
        'version' : py_version,
        'include' : include_path,
        'header' : header,
        'library' : lib_file
    }

def _compile(bdist_wheel=False):
    # check the installation of `swig`
    try:
        subprocess.call(["swig", "-version"])
    except OSError as e:
        print('%s. Please check your SWIG installation.'%e)
        return 

    info = get_py_info()
    lib_file = info['library']
    include_path = info['include']
    print(lib_file)
    
    if bdist_wheel:
        if not os.path.exists('IOHexperimenter/lib'):
            os.makedirs('IOHexperimenter/lib')

        copy(lib_file, 'IOHexperimenter/lib')
        lib_file = os.path.join(
            './IOHexperimenter/lib',
            os.path.basename(lib_file)
        )

    with open('Makefile.in') as f:
        lines = f.readlines()
        _lib = [(i, l) for i, l in enumerate(lines) if re.match(r'^py_lib=.*', l)]
        for i, l in _lib:
            lines[i] = re.sub('py_lib=', 'py_lib=%s'%lib_file, l)

        _include = [(i, l) for i, l in enumerate(lines) if re.match(r'^py_include=.*', l)]
        for i, l in _include:
            lines[i] = re.sub('py_include=', 'py_include=-I%s'%include_path, l)

        lines = [re.sub(r'(py_lib=)', r'\1%s'%lib_file, l) for l in lines]

    with open('Makefile') as f:
        f.writelines(lines)

    # command = 'sed -e "s|py_lib=|py_lib=%s|g"\
    #     -e "s|-I/python-header|-I%s|g" Makefile.in > Makefile'%(
    #         lib_file, include_path
    #     )

    os.system('make')
    
    move('_IOHprofiler.so', 'IOHexperimenter/_IOHprofiler.so')
    move('IOHprofiler.py', 'IOHexperimenter/IOHprofiler.py')

class bdist_wheel(_bdist_wheel):
    def run(self):
        _compile(bdist_wheel=True)
        super(bdist_wheel, self).run()

    def finalize_options(self):
        _bdist_wheel.finalize_options(self)
        self.root_is_pure = False

class CustomBuild(build):
    def run(self):
        set_trace()
        _compile()
        super(CustomBuild, self).run()


with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    cmdclass={
        'build': CustomBuild,
        'bdist_wheel': bdist_wheel
    },
    name="IOHexperimenter",
    version="0.2.3",
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