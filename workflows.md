

You might get this error message:
```sh
CMake Error at CMakeLists.txt:52 (add_subdirectory):
  The source directory

    /home/dimitri/Selbstgemachte_Software/IOHexperimenter/external/fmt

  does not contain a CMakeLists.txt file.
```

In this case, run:
```sh
git submodule
git submodule init
git submodule update
```

In fact, the first, of the three commands above, will give:
```sh
-32e70c1b3454a9411de2ae8d23020e08f5381f11 external/MkLandscape
-1dcb44e79a17e703e024594487b3a442d87e4741 external/cxxopts
-7df30f91aee5444a733cec0b911d21cebdeb62ae external/fmt
-6a7ed316a5cdc07b6d26362c90770787513822d4 external/googletest
-bc889afb4c5bf1c0d8ee29ef35eaaf4c8bef8a5d external/json
-80dc998efced8ceb2be59756668a7e90e8bef917 external/pybind11
```

```sh
# parentheses only work in the fish shell
sudo chown -R (id -un):(id -gn) /home/dimitri/Selbstgemachte_Software/IOHexperimenter/
sudo chmod -R 700 /home/dimitri/Selbstgemachte_Software/IOHexperimenter/
```

```sh
git clone git@github.com:Habimm/IOHexperimenter.git
git submodule
git submodule init
git submodule update
sudo chown -R (id -un):(id -gn) /home/dimitri/Selbstgemachte_Software/IOHexperimenter/
sudo chmod -R 700 /home/dimitri/Selbstgemachte_Software/IOHexperimenter/
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=./IOHexperimenter_headers ..
cd ..
sudo make install
```

To search for files under the current directory tree with a specific content:
```sh
grep -r "local_ioh"
```

To search for files under the current directory tree with a specific filename:
```sh
find . -iname "*local_ioh*"
```

To compile a file that uses the IOHexperimenter problems:
```sh
g++ -std=c++17 -I../external/fmt/include -I../include -o one_max one_max.cpp
```

```sh
set project_root /home/dimitri/code/IOHexperimenter
set fmt_include_path $project_root/external/fmt/include
set ioh_include_path $project_root/include

g++ -o one_max -g -std=c++17 -I$fmt_include_path -I$ioh_include_path one_max.cpp
./one_max
```

```sh
g++ -o one_max -g -std=c++17 -I$fmt_include_path -I$ioh_include_path one_max.cpp; and ./one_max
```

g++ version
```
g++ 9.4.0
```

Build everything:
```sh
git clone git@github.com:Habimm/IOHexperimenter.git
cd IOHexperimenter
git submodule
git submodule init
git submodule update
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=./IOHexperimenter_headers ..
cd ..
sudo make install
```

Single Objective Bound Constrained Benchmark
CEC functions
definitions
pdf
```sh
https://github.com/P-N-Suganthan/2022-SO-BO
```

Prompt for writing CEC functions
```sh
void levy_func (double *x, double *f, int nx, double *Os,double *Mr, int s_flag, int r_flag) /* Levy */
{
    int i;
  f[0] = 0.0;
  sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag); /* shift and rotate */

  double *w;
  w=(double *)malloc(sizeof(double)  *  nx);

  double sum1= 0.0;
  for (i=0; i<nx; i++)
  {
     w[i] = 1.0 + (z[i] - 0.0)/4.0;
  }

  double term1 = pow((sin(PI*w[0])),2);
  double term3 = pow((w[nx-1]-1),2) * (1+pow((sin(2*PI*w[nx-1])),2));

  double sum = 0.0;

  for (i=0; i<nx-1; i++)
  {
    double wi = w[i];
        double newv = pow((wi-1),2) * (1+10*pow((sin(PI*wi+1)),2));
    sum = sum + newv;
  }

  f[0] = term1 + sum + term3;// - 1.442600987052770; // - 1.442600987052770
  free(w);   // ADD THIS LINE to free memory! Thanks for Dr. Janez
}
rewrite this in the style of this:
        double evaluate(const std::vector<double> &x) override
        {
            auto sum1 = 0.0, sum2 = 0.0;
            for (const auto xi : x)
            {
                sum1 += cos(2.0 * IOH_PI * xi);
                sum2 += xi * xi;
            }
            if (std::isinf(sum2)) { return sum2; }

            auto result = 10.0 * (static_cast<double>(x.size()) - sum1) + sum2;
            std::cout << "result: " << result << std::endl;

            return result;
        }
ignore sr_func, further down the line replace z with x, the f will be returned as a double rather than its memory changed with a pointer
```

lint C++-17 code
```sh
# put the .clang-format file in the same directory
clang-format -i -style=llvm functions.hpp
```

Test Python bindings
```sh
conda activate ./.conda_environment
pip install -e .
ipython3
from ioh import problem
help(problem)
```

```sh
sudo apt install doxygen

conda activate ./.conda_environment
pip install breathe xmltodict sphinx sphinx-automodapi furo

cd /home/dimitri/code/IOHexperimenter/build
cmake -DBUILD_DOCS=ON ..
make doc
cd ..
ipython3 doc/generate_docs.py
```

```sh
true
git clone git@github.com-Habimm:Habimm/IOHexperimenter.git
cd IOHexperimenter
and git submodule
and git submodule init
and git submodule update
and . INSTALL
and conda activate ./.conda_environment
and pip install .
and cd ~
and ipython3 /home/dimitri/code/IOHexperimenter/tests/python/test_cec_functions.py
```

On a freshly cloned repo:
```sh
Step 1: Clone repo.
Step 2: Update git submodules.
Step 3: Install virtual environment.
Step 4: Install ioh package.
Step 5: Create .env file with a path to the static/ folder.
Step 6: Source .env.
Step 7: Run Python script.
```

```sh
echo "IOH_RESOURCES=/home/dimitri/code/IOHexperimenter/static" > .env
for line in (cat .env)
  set -x (echo $line | cut -d '=' -f 1) (echo $line | cut -d '=' -f 2-)
end
```

```sh
true
and git clone git@github.com-Habimm:Habimm/IOHexperimenter.git
and cd IOHexperimenter
and git submodule
and git submodule init
and git submodule update
and ./INSTALL_IOH
ln -fs (pwd)/static/cec_transformations build/tests/input_data
and echo "IOH_RESOURCES=/home/dimitri/code/IOHexperimenter/static" > .env
and ./RUN
```

```sh
./INSTALL_IOH
ln -fs (pwd)/static/cec_transformations build/tests/input_data
./RUN
```

Compilers
```sh
sudo apt install gcc
sudo apt install gcc-13

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 60 --slave /usr/bin/g++ g++ /usr/bin/g++-13
sudo update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-13 60
```

Compilation process:
```sh
dimitri@habimm ~/c/I/build (dynamic-bin-val)> sudo make install
[sudo] password for dimitri:
[  2%] Built target fmt
[  3%] Built target gtest
[ 27%] Built target test_ioh
[ 29%] Built target test_common
[ 32%] Built target test_examples
[ 36%] Built target test_samplers
[ 38%] Built target test_experiment
[ 40%] Built target test_analyzer
[ 42%] Built target test_eaf
[ 44%] Built target test_eah-distrib
[ 47%] Built target test_eah-scales
[ 50%] Built target test_eah-stats
[ 53%] Built target test_eah
[ 56%] Built target test_flatfile
[ 58%] Built target test_properties
[ 60%] Built target test_store
[ 63%] Built target test_triggers
[ 67%] Built target test_bbob_affine
[ 69%] Built target test_bbob_problem
[ 72%] Built target test_bbob_sbox
[ 75%] Built target test_constraints
[ 77%] Built target test_multiobjective
[ 80%] Built target test_pbo_problem
[ 82%] Built target test_star_discrepancy_integer
[ 86%] Built target test_star_discrepancy_real
[ 89%] Built target test_submodular
[ 91%] Built target test_wmodel_problem
[ 94%] Built target test_wrap_problem
[ 96%] Built target test_suite
[ 98%] Built target example_ioh
[100%] Built target eafh
Install the project...
-- Install configuration: ""
-- Installing: /usr/local/lib/libfmt.a
-- Installing: /usr/local/include/fmt/args.h
-- Installing: /usr/local/include/fmt/chrono.h
-- Installing: /usr/local/include/fmt/color.h
-- Installing: /usr/local/include/fmt/compile.h
-- Installing: /usr/local/include/fmt/core.h
-- Installing: /usr/local/include/fmt/format.h
-- Installing: /usr/local/include/fmt/format-inl.h
-- Installing: /usr/local/include/fmt/os.h
-- Installing: /usr/local/include/fmt/ostream.h
-- Installing: /usr/local/include/fmt/printf.h
-- Installing: /usr/local/include/fmt/ranges.h
-- Installing: /usr/local/include/fmt/std.h
-- Installing: /usr/local/include/fmt/xchar.h
-- Installing: /usr/local/lib/cmake/fmt/fmt-config.cmake
-- Installing: /usr/local/lib/cmake/fmt/fmt-config-version.cmake
-- Installing: /usr/local/lib/cmake/fmt/fmt-targets.cmake
-- Installing: /usr/local/lib/cmake/fmt/fmt-targets-noconfig.cmake
-- Installing: /usr/local/lib/pkgconfig/fmt.pc
-- Up-to-date: /usr/local/include
-- Up-to-date: /usr/local/include/ioh
-- Installing: /usr/local/include/ioh/common.hpp
-- Up-to-date: /usr/local/include/ioh/problem
-- Up-to-date: /usr/local/include/ioh/problem/star_discrepancy
-- Installing: /usr/local/include/ioh/problem/star_discrepancy/common.hpp
-- Installing: /usr/local/include/ioh/problem/star_discrepancy/integer.hpp
-- Installing: /usr/local/include/ioh/problem/star_discrepancy/real.hpp
-- Installing: /usr/local/include/ioh/problem/dynamic_bin_val.hpp
-- Installing: /usr/local/include/ioh/problem/constraints.hpp
-- Installing: /usr/local/include/ioh/problem/pbo.hpp
-- Installing: /usr/local/include/ioh/problem/bbob.hpp
-- Up-to-date: /usr/local/include/ioh/problem/pbo
-- Installing: /usr/local/include/ioh/problem/pbo/linear.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/ising_torus.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/leading_ones_neutrality.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/ising_ring.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/one_max_epistasis.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/leading_ones_epistasis.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/labs.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/n_queens.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/one_max.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/pbo_problem.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/leading_ones_dummy2.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/one_max_ruggedness1.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/one_max_dummy1.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/concatenated_trap.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/mis.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/leading_ones.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/nk_landscapes.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/one_max_dummy2.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/leading_ones_dummy1.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/leading_ones_ruggedness2.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/leading_ones_ruggedness1.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/one_max_ruggedness2.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/one_max_ruggedness3.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/one_max_neutrality.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/leading_ones_ruggedness3.hpp
-- Installing: /usr/local/include/ioh/problem/pbo/ising_triangular.hpp
-- Up-to-date: /usr/local/include/ioh/problem/wmodel
-- Installing: /usr/local/include/ioh/problem/wmodel/wmodel_one_max.hpp
-- Installing: /usr/local/include/ioh/problem/wmodel/wmodel_problem.hpp
-- Installing: /usr/local/include/ioh/problem/wmodel/wmodel_leading_ones.hpp
-- Installing: /usr/local/include/ioh/problem/wmodel/README.md
-- Installing: /usr/local/include/ioh/problem/dynamic_bin_val
-- Installing: /usr/local/include/ioh/problem/dynamic_bin_val/dynamic_bin_val.hpp
-- Installing: /usr/local/include/ioh/problem/wrap_function.hpp
-- Installing: /usr/local/include/ioh/problem/problem.hpp
-- Up-to-date: /usr/local/include/ioh/problem/mklandscape
-- Installing: /usr/local/include/ioh/problem/mklandscape/README.md
-- Installing: /usr/local/include/ioh/problem/mklandscape/cliqueTreeC.hpp
-- Installing: /usr/local/include/ioh/problem/single.hpp
-- Installing: /usr/local/include/ioh/problem/structures.hpp
-- Installing: /usr/local/include/ioh/problem/.gitignore
-- Installing: /usr/local/include/ioh/problem/transformation.hpp
-- Up-to-date: /usr/local/include/ioh/problem/bbob
-- Installing: /usr/local/include/ioh/problem/bbob/griewank_rosenbrock.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/weierstrass.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/step_ellipsoid.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/bbob_problem.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/bueche_rastrigin.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/many_affine.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/schaffers10.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/gallagher101.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/rosenbrock.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/ellipsoid_rotated.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/gallagher21.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/rastrigin_rotated.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/lunacek_bi_rastrigin.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/attractive_sector.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/bent_cigar.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/sharp_ridge.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/linear_slope.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/sphere.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/ellipsoid.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/rastrigin.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/katsuura.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/discus.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/rosenbrock_rotated.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/different_powers.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/schaffers1000.hpp
-- Installing: /usr/local/include/ioh/problem/bbob/schwefel.hpp
-- Up-to-date: /usr/local/include/ioh/problem/submodular
-- Installing: /usr/local/include/ioh/problem/submodular/pack_while_travel.hpp
-- Installing: /usr/local/include/ioh/problem/submodular/graph_problem.hpp
-- Installing: /usr/local/include/ioh/problem/submodular/max_coverage.hpp
-- Installing: /usr/local/include/ioh/problem/submodular/max_influence.hpp
-- Installing: /usr/local/include/ioh/problem/submodular/max_cut.hpp
-- Installing: /usr/local/include/ioh/problem/utils.hpp
-- Installing: /usr/local/include/ioh/problem/submodular.hpp
-- Installing: /usr/local/include/ioh/problem/wmodel.hpp
-- Up-to-date: /usr/local/include/ioh/common
-- Installing: /usr/local/include/ioh/common/optimization_type.hpp
-- Installing: /usr/local/include/ioh/common/timer.hpp
-- Installing: /usr/local/include/ioh/common/file.hpp
-- Installing: /usr/local/include/ioh/common/format.hpp
-- Installing: /usr/local/include/ioh/common/clutchlog.h
-- Installing: /usr/local/include/ioh/common/factory.hpp
-- Installing: /usr/local/include/ioh/common/repr.hpp
-- Installing: /usr/local/include/ioh/common/log.hpp
-- Installing: /usr/local/include/ioh/common/random.hpp
-- Installing: /usr/local/include/ioh/common/sobol.hpp
-- Installing: /usr/local/include/ioh/common/sampler.hpp
-- Installing: /usr/local/include/ioh/common/config.hpp
-- Installing: /usr/local/include/ioh/common/container_utils.hpp
-- Installing: /usr/local/include/ioh/problem.hpp
-- Installing: /usr/local/include/ioh/logger.hpp
-- Installing: /usr/local/include/ioh/experiment.hpp
-- Up-to-date: /usr/local/include/ioh/logger
-- Installing: /usr/local/include/ioh/logger/eaf.hpp
-- Installing: /usr/local/include/ioh/logger/analyzer.hpp
-- Installing: /usr/local/include/ioh/logger/store.hpp
-- Installing: /usr/local/include/ioh/logger/combine.hpp
-- Installing: /usr/local/include/ioh/logger/eah.hpp
-- Installing: /usr/local/include/ioh/logger/loggers.hpp
-- Installing: /usr/local/include/ioh/logger/triggers.hpp
-- Installing: /usr/local/include/ioh/logger/flatfile.hpp
-- Installing: /usr/local/include/ioh/logger/properties.hpp
-- Installing: /usr/local/include/ioh/logger/loginfo.hpp
-- Installing: /usr/local/include/ioh/suite.hpp
-- Installing: /usr/local/include/README.md
-- Installing: /usr/local/include/ioh.hpp
-- Installing: /usr/local/lib/cmake/ioh/ioh-config.cmake
-- Installing: /usr/local/lib/cmake/ioh/ioh-config-version.cmake
-- Installing: /usr/local/lib/cmake/ioh/ioh-targets.cmake
```

```sh
ssh mesu

module add cmake/3.22
module add gcc/11.2

git clone git@github.com:IOHprofiler/IOHexperimenter.git
git checkout dynamic-bin-val
cd IOHexperimenter
git submodule
git submodule init
git submodule update
mkdir build
cd build
cmake ..
```

Using clang rather than gcc as a compiler:
```sh
module add cmake/3.22
module add gcc/11.2
module add LLVM/clang-llvm-10.0

wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
sudo add-apt-repository "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-10 main"
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 15CF4D18AF4F7421
sudo apt update
sudo apt install clang-10 lldb-10 lld-10

sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-10 100
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-10 100
sudo update-alternatives --config clang
sudo update-alternatives --config clang++
CC=clang CXX=clang++ cmake -DCMAKE_CXX_FLAGS="-stdlib=libc++" -DCMAKE_INSTALL_PREFIX=IOHexperimenter ..

make install
```

```sh
ssh mesu

module add cmake/3.22
module add conda3-2023.02
module add gcc/11.2
module add LLVM/clang-llvm-10.0

git clone git@github.com:IOHprofiler/IOHexperimenter.git
git checkout dynamic-bin-val
cd IOHexperimenter
git submodule
git submodule init
git submodule update
mkdir build
cd build
CC=clang CXX=clang++ cmake -DCMAKE_CXX_FLAGS="-stdlib=libc++" -DCMAKE_INSTALL_PREFIX=IOHexperimenter ..

make install
```

If clang does not work because of an error with "linker":
```sh
sudo apt install libc++-dev libc++abi-dev
```

```sh
CC=clang CXX=clang++ cmake -DCMAKE_CXX_FLAGS="-stdlib=libc++" -DCMAKE_INSTALL_PREFIX=IOHexperimenter ..
CC=clang CXX=clang++ pip install .
CC=clang CXX=clang++ cmake -DCMAKE_INSTALL_PREFIX=IOHexperimenter ..
```

```sh
ln -fs /usr/lib/x86_64-linux-gnu/libstdc++.so.6 /home/dimitri/code/IOHexperimenter/.conda_environment/lib/libstdc++.so.6
```

```sh
conda activate base
rm -rf ./.conda_environment
conda env create --prefix ./.conda_environment --file conda.yaml
conda activate ./.conda_environment

pip install .
```
