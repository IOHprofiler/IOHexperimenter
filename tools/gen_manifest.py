"""Generates static/manifest.json and src/data_manifest.inc, or with --check, checks static/ against them.

The manifest lists every data file in static/ (path, size, sha256) and a data version (a hash of that list), and
the registry entries defined by data: the graph problems (MaxCut, MaxCoverage, MaxInfluence, PackWhileTravel) and the
CEC2013 problems, each with its id, registry name, meta name, files and definition (graphs). The entries were generated once, with --entries, from the golden fixtures (tests/data/golden) and the
definition files. Since then they are kept as they are, so that the ids of the built-in problems never again depend
on the order of the lines in a definitions file; without --entries only the files and the data version are updated.
The library is compiled with the same entries, files and data version (src/data_manifest.inc), so that registering
the built-in problems needs no data, and it checks the data version of the data directory it reads.

usage: python tools/gen_manifest.py [--entries | --check | --check-package FILE...]

--check-package checks that wheels or sdists hold exactly the data files of the manifest (and the manifest).
"""
import argparse
import hashlib
import json
import pathlib
import re
import sys
import tarfile
import zipfile

ROOT = pathlib.Path(__file__).resolve().parent.parent
STATIC = ROOT / "static"
MANIFEST = STATIC / "manifest.json"
COMPILED = ROOT / "src" / "data_manifest.inc"
GOLDEN = ROOT / "tests" / "data" / "golden"
IGNORED = {".DS_Store", MANIFEST.name}

# the graph classes: their first id and their definitions file (the id of a graph was its class's first id plus
# its line in that file)
GRAPH_CLASSES = (
    ("MaxCut", 2000, "example_list_maxcut"),
    ("MaxCoverage", 2100, "example_list_maxcoverage"),
    ("MaxInfluence", 2200, "example_list_maxinfluence"),
    ("PackWhileTravel", 2300, "example_list_pwt"),
)
CEC2013_DATA = "cec_transformations/2013/"


def data_files():
    files = []
    for path in sorted(p for p in STATIC.rglob("*") if p.is_file() and p.name not in IGNORED):
        content = path.read_bytes()
        files.append({
            "path": path.relative_to(STATIC).as_posix(),
            "size": len(content),
            "sha256": hashlib.sha256(content).hexdigest(),
        })
    return files


def data_version(files):
    listing = "".join(f"{f['path']}\t{f['sha256']}\n" for f in files)
    return hashlib.sha256(listing.encode()).hexdigest()[:16]


def is_number(text):
    try:
        float(text)
        return True
    except ValueError:
        return False


def golden_records():
    registry = json.loads((GOLDEN / "registry.json").read_text())["problem_classes"]
    records = {}
    for key, record in json.loads((GOLDEN / "behaviour.json").read_text()).items():
        family, pid = key.split("/")[:2]
        if family in ("GRAPH", "CEC2013"):
            records.setdefault((family, int(pid)), record)
    return registry, records


def golden_meta(records, family, pid):
    """The meta name and dimension of a problem with a fixed dimension and instance"""
    record = records[(family, pid)]
    assert record["fixed_dimension"] is True and record["fixed_instance"] is True, (family, pid)
    return record["meta"]["name"], record["meta"]["n_variables"]


def generate_entries():
    registry, records = golden_records()
    graphs = []
    for cls, first_id, definitions in GRAPH_CLASSES:
        lines = [line.strip() for line in (STATIC / definitions).read_text().splitlines() if line.strip()]
        for index, definition in enumerate(lines):
            pid = first_id + index
            name = registry["GRAPH"][str(pid)]
            meta_name, _ = golden_meta(records, "GRAPH", pid)
            assert name == meta_name == f"{cls}{pid}", (pid, name, meta_name)
            # edge file|edge weights|vertex weights|constraint weights|constraint variances, the last a file or a
            # number (a pack-while-travel definition is one file)
            files = [f for i, f in enumerate(definition.split("|")) if f and f != "NULL" and not (i == 4 and is_number(f))]
            graphs.append({"id": pid, "name": name, "class": cls, "definition": definition, "files": files})
    assert len(graphs) == len(registry["GRAPH"])

    cec2013 = []
    for pid, name in sorted((int(k), v) for k, v in registry["CEC2013"].items()):
        meta_name, dimension = golden_meta(records, "CEC2013", pid)
        files = []
        composition = re.fullmatch(r"CEC2013CF(\d+)D(\d+)", meta_name)
        if composition:
            cf, d = map(int, composition.groups())
            files.append(f"{CEC2013_DATA}optima.dat")
            if cf >= 3:  # CF1 and CF2 have identity transformations
                files.append(f"{CEC2013_DATA}CF{cf}_M_D{d}.dat")
            assert d == dimension, (pid, meta_name, dimension)
        cec2013.append({"id": pid, "name": name, "meta_name": meta_name, "files": files})
    return {"GraphProblem": graphs, "CEC2013": cec2013}


def compiled(manifest):
    """The manifest as C++, included by src/data.cpp"""
    def literal(text):
        return json.dumps(text)  # a valid C++ string literal for these (ASCII) texts

    lines = ["// Generated by tools/gen_manifest.py from static/manifest.json: do not edit", "",
             f"const char *const generated_data_version = {literal(manifest['data_version'])};", "",
             "const char *const generated_files[] = {"]
    lines += [f"    {literal(f['path'])}," for f in manifest["files"]]
    lines += ["};", "", "// family, id, name, meta name, class, definition",
              "const GeneratedEntry generated_entries[] = {"]
    for family, entries in manifest["entries"].items():
        for e in entries:
            fields = [literal(family), str(e["id"]), literal(e["name"]), literal(e.get("meta_name", e["name"])),
                      literal(e.get("class", "")), literal(e.get("definition", ""))]
            lines.append(f"    {{{', '.join(fields)}}},")
    lines.append("};")
    return "\n".join(lines) + "\n"


def write(entries):
    files = data_files()
    manifest = {"format": 1, "data_version": data_version(files), "files": files, "entries": entries}
    MANIFEST.write_text(json.dumps(manifest, indent=1) + "\n")
    COMPILED.write_text(compiled(manifest))
    print(f"wrote {MANIFEST.relative_to(ROOT)}: {len(files)} files, "
          f"{sum(len(v) for v in entries.values())} entries, data version {manifest['data_version']}")


def check():
    manifest = json.loads(MANIFEST.read_text())
    files = data_files()
    listed = {f["path"]: f for f in manifest["files"]}
    actual = {f["path"]: f for f in files}
    problems = [f"not in static/: {p}" for p in sorted(listed.keys() - actual.keys())]
    problems += [f"not in the manifest: {p}" for p in sorted(actual.keys() - listed.keys())]
    problems += [f"size or sha256 differs: {p}" for p in sorted(listed.keys() & actual.keys()) if listed[p] != actual[p]]
    if manifest["data_version"] != data_version(manifest["files"]):
        problems.append("the data version does not match the files")
    for family, entries in manifest["entries"].items():
        for entry in entries:
            problems += [f"{family} {entry['id']}: missing file {f}" for f in entry["files"] if f not in actual]
    if manifest["entries"] != generate_entries():
        problems.append("the entries differ from the golden fixtures and the definition files")
    if not COMPILED.exists() or COMPILED.read_text() != compiled(manifest):
        problems.append(f"{COMPILED.relative_to(ROOT)} differs from the manifest")
    for problem in problems:
        print(problem)
    if problems:
        print("static/ does not match static/manifest.json: run python tools/gen_manifest.py (and see its --help)")
        return 1
    print(f"static/ matches static/manifest.json ({len(files)} files, data version {manifest['data_version']})")
    return 0


def check_package(archive):
    """Checks that a wheel or an sdist holds exactly the manifest's files, in a static folder"""
    archive = pathlib.Path(archive)
    if archive.suffix == ".whl":
        with zipfile.ZipFile(archive) as z:
            contents = {name: z.read(name) for name in z.namelist()
                        if not name.endswith("/") and "/static/" in f"/{name}"}
    else:
        with tarfile.open(archive) as t:
            contents = {m.name: t.extractfile(m).read() for m in t.getmembers()
                        if m.isfile() and "/static/" in f"/{m.name}"}
    prefixes = {name[: name.index("static/") + len("static/")] for name in contents}
    if len(prefixes) != 1:
        print(f"{archive.name}: expected one static folder, found {sorted(prefixes) or 'none'}")
        return 1
    prefix = prefixes.pop()
    packed = {name[len(prefix):]: data for name, data in contents.items()}
    manifest = json.loads(packed.pop(MANIFEST.name, b"{}") or b"{}")
    if manifest != json.loads(MANIFEST.read_text()):
        print(f"{archive.name}: {prefix}{MANIFEST.name} is missing or differs from {MANIFEST.relative_to(ROOT)}")
        return 1
    listed = {f["path"]: f for f in manifest["files"]}
    problems = [f"missing: {p}" for p in sorted(listed.keys() - packed.keys())]
    problems += [f"not in the manifest: {p}" for p in sorted(packed.keys() - listed.keys())]
    problems += [f"differs: {p}" for p in sorted(listed.keys() & packed.keys())
                 if hashlib.sha256(packed[p]).hexdigest() != listed[p]["sha256"]]
    for problem in problems:
        print(f"{archive.name}: {problem}")
    if not problems:
        print(f"{archive.name}: {prefix} holds exactly the manifest's {len(listed)} files")
    return 1 if problems else 0


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--entries", action="store_true", help="also generate the entries (done once)")
    group.add_argument("--check", action="store_true", help="check static/ against the manifest")
    group.add_argument("--check-package", nargs="+", metavar="FILE", help="check wheels or sdists")
    args = parser.parse_args()
    if args.check:
        sys.exit(check())
    if args.check_package:
        sys.exit(max(check_package(archive) for archive in args.check_package))
    write(generate_entries() if args.entries or not MANIFEST.exists() else json.loads(MANIFEST.read_text())["entries"])
