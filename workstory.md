




```sh
pip install -e . && python -m unittest
cmake --build . --target test_ioh && ./tests/test_ioh

rm -rf build
mkdir build
cd build
cmake ..

cmake --build . --target test_dynamic_bin_val && ./tests/test_dynamic_bin_val
```

```sh
git restore --source other-branch --worktree -- path/to/your/file
git restore --source workstory --worktree -- workstory.md
```
