# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

## Monkey patched modules
## line 254 for automodapi.py: list(map(str.strip, args.strip().split(", ")))
## setup for breathe, removed graph_viz setup

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
sys.path.insert(0, os.path.abspath('..'))


# -- Project information -----------------------------------------------------

project = 'IOHExperimenter'
copyright = '2022, Jacob de Nobel, Furong Ye, Diederick Vermetten, Hao Wang, Carola Doerr and Thomas Bäck'
author = 'Jacob de Nobel, Furong Ye, Diederick Vermetten, Hao Wang, Carola Doerr and Thomas Bäck'

# The full version, including alpha/beta/rc tags
with open(os.path.realpath(os.path.join(os.path.dirname(__file__), "../../..", "VERSION"))) as f:
    release = f.read().strip()


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.coverage',
    'sphinx.ext.napoleon',
    'sphinx.ext.autosummary',
    'sphinx_automodapi.automodapi',
    'sphinx.ext.graphviz',
    'sphinx_automodapi.smart_resolver',
    'sphinx.ext.intersphinx',
    'sphinx.ext.imgmath',
    'sphinx.ext.todo', 
    'breathe',
    'm2r2'
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []
numpydoc_show_class_members = False

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'furo'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

github_url = 'https://github.com/IOHprofiler/IOHexperimenter'

autoclass_content = 'both'

autodoc_default_options = {
}

master_doc = "contents"


breathe_projects = {
    "iohcpp": "../../build/xml/",
}

breathe_default_project = "iohcpp"

source_suffix = ['.rst', '.md']

breathe_default_members = ('members', 'undoc-members', 'private-members', 'protected-members', 'allow-dot-graphs')


