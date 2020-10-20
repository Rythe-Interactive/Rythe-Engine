"""Sphinx Nameko Theme.

A fork of https://github.com/ignacysokolowski/sphinx-readable-theme for use
in Nameko (https://github.com/onefinestay/nameko)

"""

import os


def get_html_theme_path():
    """Return path to directory containing package theme."""
    return os.path.abspath(os.path.dirname(__file__))
