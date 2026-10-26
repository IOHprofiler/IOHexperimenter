"""Builds static.tar.gz from static/, or with --check, checks that static.tar.gz holds exactly the files of static/.

Released versions (0.3.22 and older) download static.tar.gz from master (TASKS.md, section 1), so it is rebuilt
whenever static/ changes. The archive is reproducible: sorted entries, fixed times and owners.

usage: python tools/static_tarball.py [--check]
"""
import argparse
import gzip
import io
import pathlib
import sys
import tarfile

ROOT = pathlib.Path(__file__).resolve().parent.parent
STATIC = ROOT / "static"
TARBALL = ROOT / "static.tar.gz"
IGNORED = {".DS_Store"}


def archive_name(path):
    return pathlib.PurePosixPath("static", *path.relative_to(STATIC).parts).as_posix()


def static_paths():
    return sorted(p for p in STATIC.rglob("*") if p.name not in IGNORED)


def build():
    buffer = io.BytesIO()
    with tarfile.open(fileobj=buffer, mode="w", format=tarfile.PAX_FORMAT) as tar:
        for path in [STATIC] + static_paths():
            info = tar.gettarinfo(str(path), arcname=archive_name(path))
            info.mtime, info.uid, info.gid, info.uname, info.gname = 0, 0, 0, "", ""
            info.mode = 0o755 if path.is_dir() else 0o644
            if path.is_file():
                with open(path, "rb") as f:
                    tar.addfile(info, f)
            else:
                tar.addfile(info)
    with open(TARBALL, "wb") as out, gzip.GzipFile(filename="", mode="wb", fileobj=out, mtime=0) as gz:
        gz.write(buffer.getvalue())


def check():
    with tarfile.open(TARBALL) as tar:
        packed = {m.name: tar.extractfile(m).read() for m in tar.getmembers() if m.isfile()}
    files = {archive_name(p): p.read_bytes() for p in static_paths() if p.is_file()}
    problems = [f"only in static.tar.gz: {name}" for name in sorted(packed.keys() - files.keys())]
    problems += [f"only in static/: {name}" for name in sorted(files.keys() - packed.keys())]
    problems += [f"differs: {name}" for name in sorted(packed.keys() & files.keys()) if packed[name] != files[name]]
    for problem in problems:
        print(problem)
    if problems:
        print("static.tar.gz does not match static/: run python tools/static_tarball.py")
        return 1
    print(f"static.tar.gz matches static/ ({len(files)} files)")
    return 0


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--check", action="store_true", help="check instead of building")
    sys.exit(check() if parser.parse_args().check else build())
