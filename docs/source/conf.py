# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'ArgsEngine'
copyright = '2019, Raphael Baier, Args-Engine Team'
author = 'Raphael Baier'

# The full version, including alpha/beta/rc tags
release = 'v 0.2 alpha'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "breathe",
    "recommonmark",
    "exhale"
]

breathe_projects = {
    "ArgsEngine": "../xml/",
}

source_suffix = {
    '.rst': 'restructuredtext',
    '.txt': 'markdown',
    '.md': 'markdown',
    '.MD': 'markdown',
}

exhale_args = {

    "containmentFolder": "./api",
    "rootFileName": "library_root.rst",
    "rootFileTitle": "ARGS API",
    "doxygenStripFromPath": "..",
    "createTreeView": True,
}

primary_domain = 'cpp'

highlight_language = 'cpp'

cpp_id_attributes = ["A_NODISCARD"]

breathe_default_project = "ArgsEngine"

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []

# -- Options for HTML output -------------------------------------------------

# we are using a local theme, to not confuse
# python we need to insert ./ into python path
import os,sys

here = os.path.abspath(os.path.dirname(__file__))
sys.path.insert(0,here)

# our custom theme is basically just nameko with different colours
# we basically just need to change the template path
html_theme = 'nameko'

# now we can import our custom theme
import argstheme

html_theme_path = [argstheme.get_html_theme_path()]
