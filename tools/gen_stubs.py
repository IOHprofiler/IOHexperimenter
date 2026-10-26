"""Regenerates the type stubs of the package (ioh/__init__.pyi and ioh/iohcpp/): python tools/gen_stubs.py

Needs an editable install of the package (pip install -e .) and mypy (pip install mypy) in the interpreter
that runs it. Until WP5 the build did this when MAKE_STUBS was set.
"""
import importlib.util
import os
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.realpath(os.path.join(os.path.dirname(__file__), ".."))


def main():
    if importlib.util.find_spec("mypy") is None:
        sys.exit("gen_stubs.py needs mypy: pip install mypy")
    spec = importlib.util.find_spec("ioh")
    if spec is None or os.path.realpath(spec.origin) != os.path.join(ROOT, "ioh", "__init__.py"):
        sys.exit(f"gen_stubs.py needs an editable install of {ROOT}: pip install -e .")

    with tempfile.TemporaryDirectory() as out:
        # mypy is compiled, so stubgen cannot run with -m
        command = [sys.executable, "-c", "from mypy.stubgen import main; main()", "-m", "ioh", "-p", "ioh.iohcpp",
                   "-o", out]
        subprocess.check_call(command, cwd=ROOT)

        # replace the stubs once the new ones are there
        for stubfile in os.listdir(os.path.join(ROOT, "ioh")):
            if stubfile.endswith(".pyi"):
                os.remove(os.path.join(ROOT, "ioh", stubfile))
        for subdir in ("iohcpp", "logger"):
            if os.path.isdir(os.path.join(ROOT, "ioh", subdir)):
                shutil.rmtree(os.path.join(ROOT, "ioh", subdir))
        shutil.copytree(os.path.join(out, "ioh"), os.path.join(ROOT, "ioh"), dirs_exist_ok=True)


if __name__ == "__main__":
    main()
