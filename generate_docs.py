import os
import collections
import subprocess
import shutil
import warnings
import xmltodict

BASE_DIR = os.path.realpath(os.path.dirname(__file__))

unit_template = '''{name}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. doxygen{dtype}:: {ns}::{name}
'''

namespace_template = '''
Namespaces
------------------
.. toctree::
    :maxdepth: 1

    {subns}
'''

template = '''
{name} 
================
{namespaces}
{types}{classes}{structs}{enums}{functions}{variables}
'''

def make_template(nsdata):
    ns = nsdata['name']
    classes = nsdata.get('classes')
    structs = nsdata.get('structs')
    functions = nsdata.get('func')
    enums = nsdata.get('enum')
    types = nsdata.get('typedef')
    variables = nsdata.get('var')
    
    if ns == 'ioh':
        nsdata['namespaces'] = None

    def formatter(dtype, title, objs):
        return '' if objs is None else f'\n{title}\n----------\n' + '\n'.join(
            unit_template.format(ns=ns, dtype=dtype, name=x.split("::")[-1]) for x in objs
        )    
   
    return template.format(
        name=ns.split("::")[-1], 
        ns=ns,
        namespaces='' if nsdata['namespaces'] is None else namespace_template.format(
            subns="\n    ".join([f'{x.replace("::", "_")}.rst' for x in nsdata['namespaces']])
        ),
        types=formatter('typedef', 'Types', types),
        classes=formatter('class', 'Classes', classes),
        structs=formatter('struct', 'Structs', structs),
        enums=formatter('enum', 'Enumerations', enums),
        functions=formatter('function', 'Functions', functions),
        variables=formatter('variable', 'Variables', variables),
    )

def get_namespace_files(xmldir):
    for f in os.listdir(xmldir):
        if not f.endswith(".xml"): continue
        if not f.startswith("namespace"): continue
        yield os.path.join(xmldir, f)

def get_values(list_of_d, attr ='#text'):
    if list_of_d is None:
        return 

    if isinstance(list_of_d, (collections.OrderedDict, dict)):
        list_of_d = [list_of_d]

    return list(sorted(set([x[attr] for x in list_of_d])))  
    
        
def generate_sphinx_templates_from_xml(xmldir = "doc/build/xml", outdir = "doc/python/source/cpp"):
    assert os.path.isdir(xmldir), xmldir
    assert os.path.isdir(outdir), outdir

    ns = {}
    for f in get_namespace_files(xmldir):
        with open(f) as h:
            data = xmltodict.parse(h.read())
        name = data['doxygen']['compounddef']['compoundname']
        namespaces = get_values(data['doxygen']['compounddef'].get('innernamespace'))
        classes = get_values(data['doxygen']['compounddef'].get('innerclass'))
        sections = data['doxygen']['compounddef'].get('sectiondef')
        nsdata = dict(name=name, namespaces=namespaces)

        if classes is not None:
            classes = []
            structs = []
            cdata = data['doxygen']['compounddef'].get('innerclass')
            if isinstance(cdata, (dict, collections.OrderedDict)):
                cdata = [cdata]
            for e in cdata:
                if e['@refid'].startswith('class'):
                    classes.append(e['#text'])
                else:
                    structs.append(e['#text'])

            nsdata['classes'] = classes if any(classes) else None
            nsdata['structs'] = structs if any(structs) else None

        
        if sections is not None:
            if isinstance(sections, (dict, collections.OrderedDict)):
                sections = [sections]
            for sec in sections:
                nsdata[sec['@kind']] = [f"{name}::{x}" for x in get_values(sec['memberdef'], "name")]

        ns[name] = nsdata

        
        filename = f'{name.replace("::", "_")}.rst'
        
        with open(os.path.join(outdir, filename), "w+") as f:
            f.write(make_template(nsdata))


def generate_stubs(sourcedir):
    # remove any existing stubs
    for stubfile in os.listdir(os.path.join(sourcedir, "ioh")):
        stubfile = os.path.join(sourcedir, "ioh", stubfile)
        if stubfile.endswith(".pyi"):
            os.remove(stubfile)

    for subdir in ("iohcpp", "logger"):
        if os.path.isdir(os.path.join(sourcedir, "ioh", subdir)):
            shutil.rmtree(os.path.join(sourcedir, "ioh", subdir))

    # generate stub files
    command = """stubgen -m ioh -p ioh.iohcpp -o ./"""
    subprocess.check_call(command, cwd=sourcedir, shell=True)


def main():
    try:
        generate_sphinx_templates_from_xml()
        directory = os.path.join(BASE_DIR, "doc", "python")
        try:
            shutil.rmtree(os.path.join(directory, "source", "api"))
        except FileNotFoundError:
            pass
        subprocess.check_call(f"make html", shell=True, cwd=directory)

        shutil.copytree(
            os.path.join(BASE_DIR, "doc", "build", "html"),
            os.path.join(BASE_DIR, "docs"),
            dirs_exist_ok=True
        )
    except Exception as reason:
        warnings.warn(f"Cannot compile docs reason: {reason}", RuntimeWarning)

if __name__ == '__main__':
    main()
        
