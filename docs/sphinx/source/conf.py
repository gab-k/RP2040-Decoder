# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import subprocess
import re

def get_latest_release_tag():
    try:
        # Get all tags matching the pattern
        tags = subprocess.check_output(
            ['git', 'tag', '--list', 'v*-RELEASE'],
            universal_newlines=True
        ).splitlines()

        if not tags:
            return "unknown"

        # Sort tags in reverse version order
        tags.sort(reverse=True)

        latest_tag = tags[0]

        # Extract version numbers (optional: you can customize this regex)
        match = re.match(r'v(\d+\.\d+)-RELEASE', latest_tag)
        if match:
            return match.group(1)
        else:
            return "unknown"

    except Exception:
        return "unknown"
    


project = 'RP2040-Decoder'
copyright = '2024, Gabriel Koppenstein'
author = 'Gabriel Koppenstein'
release = get_latest_release_tag()

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['breathe']

templates_path = ['_templates']
exclude_patterns = []



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "sphinx_rtd_theme"
html_static_path = ['_static']

# Breathe Configuration
breathe_projects = {
    "RP2040-Decoder": "../../doxygen/build/xml"
}
breathe_default_project = "RP2040-Decoder"

