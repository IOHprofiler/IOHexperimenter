"""Regenerates the type stubs of the package (ioh/__init__.pyi and ioh/iohcpp/): python tools/gen_stubs.py

The stubs of the extension module ioh.iohcpp come from nanobind's stubgen, one .pyi per (sub)module, and the stub of
the pure-Python package (ioh/__init__.pyi) from mypy's stubgen. Needs an editable install of the package
(pip install -e .), mypy, and the nanobind version that the package builds with (pyproject.toml), in the interpreter
that runs it: pip install mypy nanobind==<that version>. Until WP5 the build did this when MAKE_STUBS was set.
"""
import importlib.metadata
import importlib.util
import os
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.realpath(os.path.join(os.path.dirname(__file__), ".."))

# some of the stubs of the extension module (in ioh/), to check that the generator ran; nanobind's stubgen writes a
# module with submodules to <name>/__init__.pyi (problem has the submodule sbox)
EXTENSION_STUBS = [
    os.path.join("iohcpp", "__init__.pyi"),
    os.path.join("iohcpp", "problem", "__init__.pyi"),
    os.path.join("iohcpp", "suite.pyi"),
    os.path.join("iohcpp", "logger", "__init__.pyi"),
]


def nanobind_version():
    """Return the nanobind version that the package builds with, on which the stubs depend."""
    with open(os.path.join(ROOT, "pyproject.toml"), encoding="utf-8") as f:
        match = re.search(r"""["']nanobind==([^"';\s]+)["']""", f.read())
    if match is None:
        sys.exit("gen_stubs.py: pyproject.toml pins no nanobind version (nanobind==...) in its build requirements")
    return match.group(1)


def main():
    if importlib.util.find_spec("mypy") is None:
        sys.exit("gen_stubs.py needs mypy: pip install mypy")
    version = nanobind_version()
    try:
        installed = importlib.metadata.version("nanobind")
    except importlib.metadata.PackageNotFoundError:
        installed = None
    if installed != version:
        sys.exit(f"gen_stubs.py needs nanobind {version}, the version the package builds with "
                 f"(installed: {installed}): pip install nanobind=={version}")
    spec = importlib.util.find_spec("ioh")
    if spec is None or os.path.realpath(spec.origin) != os.path.join(ROOT, "ioh", "__init__.py"):
        sys.exit(f"gen_stubs.py needs an editable install of {ROOT}: pip install -e .")

    with tempfile.TemporaryDirectory() as out:
        mypy_out, nanobind_out = os.path.join(out, "mypy"), os.path.join(out, "nanobind")

        # the pure-Python package; mypy is compiled, so stubgen cannot run with -m
        command = [sys.executable, "-c", "from mypy.stubgen import main; main()", "-m", "ioh", "-o", mypy_out]
        subprocess.check_call(command, cwd=ROOT)  # nosec B603: a fixed command with this interpreter

        # the extension module and, recursively (-r), its submodules. It runs in the temporary directory, which has
        # no ioh directory, so that ioh is imported from the install (the stubgen puts the current directory first
        # on sys.path).
        command = [sys.executable, "-m", "nanobind.stubgen", "-q", "-r", "-m", "ioh.iohcpp",
                   "-O", os.path.join(nanobind_out, "ioh")]
        subprocess.check_call(command, cwd=out)  # nosec B603: a fixed command with this interpreter

        stubs = [os.path.join(mypy_out, "ioh", "__init__.pyi")]
        stubs += [os.path.join(nanobind_out, "ioh", stub) for stub in EXTENSION_STUBS]
        missing = [path for path in stubs if not os.path.isfile(path)]
        if missing:
            sys.exit("gen_stubs.py: the stub generators did not write " + ", ".join(missing))

        # replace the stubs once the new ones are there
        for stubfile in os.listdir(os.path.join(ROOT, "ioh")):
            if stubfile.endswith(".pyi"):
                os.remove(os.path.join(ROOT, "ioh", stubfile))
        for subdir in ("iohcpp", "logger"):
            if os.path.isdir(os.path.join(ROOT, "ioh", subdir)):
                shutil.rmtree(os.path.join(ROOT, "ioh", subdir))
        shutil.copy(os.path.join(mypy_out, "ioh", "__init__.pyi"), os.path.join(ROOT, "ioh", "__init__.pyi"))
        # all of nanobind's stubs, also of submodules added after EXTENSION_STUBS was written
        shutil.copytree(os.path.join(nanobind_out, "ioh", "iohcpp"), os.path.join(ROOT, "ioh", "iohcpp"))
        # the stubgen leaves out logger.Path, an alias of pathlib.Path (a module attribute from another module)
        with open(os.path.join(ROOT, "ioh", "iohcpp", "logger", "__init__.pyi"), "a", encoding="utf-8") as f:
            f.write("\nfrom pathlib import Path as Path\n")


if __name__ == "__main__":
    main()
