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
# import sys
# sys.path.insert(0, os.path.abspath('.'))
import os
import pathlib
import re

import requests

PROJECT_NAME = 'SustainML'
COMPRESS_PROJECT_NAME = 'sustainml'

def get_version(version_file='VERSION'):
    """
    Get the project version from a version file.

    This method get the VERSION file in this directory and read the version from it.

    :param version_file: The file to scan the version (Default: VERSION)
    :return: A dict in the manner:
        {
            'major': int,
            'minor': int,
            'patch': int
        }
        None if file does not exist
    """
    version = {}

    try:
        with open(version_file, 'r') as f:
            for line in f:
                if re.search('VERSION_MAJOR', line):
                    version['major'] = line.split()[1][:-1]
                if re.search('VERSION_MINOR', line):
                    version['minor'] = line.split()[1][:-1]
                if re.search('VERSION_PATCH', line):
                    version['patch'] = line.split()[1][:-1]
                if ('major' in version and
                        'minor' in version and
                        'patch' in version):
                    break
    except EnvironmentError:
        return None

    return version


def get_version_from_cmakelists(cmakelists):
    """
    Get the project version from a cmakelists file.

    The function looks for PRODUCT_MAJOR_VERSION, PRODUCT_MINOR_VERSION, and
    PRODUCT_PATCH_VERSION in the given file

    :param cmakelists: The file to scan for the version
    :return: A dict in the manner:
        {
            'major': int,
            'minor': int,
            'patch': int
        }
    """
    version = {}
    with open(cmakelists, 'r') as f:
        for line in f:
            if re.search('PRODUCT_MAJOR_VERSION', line):
                version['major'] = line.split()[1][:-1]
            if re.search('PRODUCT_MINOR_VERSION', line):
                version['minor'] = line.split()[1][:-1]
            if re.search('PRODUCT_PATCH_VERSION', line):
                version['patch'] = line.split()[1][:-1]
            if ('major' in version and
                    'minor' in version and
                    'patch' in version):
                break
    return version


def download_css(html_css_dir):
    """
    Download the common theme of eProsima readthedocs documentation.

    The theme is defined in a CSS file that is hosted in the eProsima GitHub
    repository with the index of all eProsima product documentation
    (https://github.com/eProsima/all-docs).

    :param html_css_dir: The directory to save the CSS stylesheet.
    :return: True if the file was downloaded and generated successfully.
        False if not.
    """
    url = (
        'https://raw.githubusercontent.com/eProsima/all-docs/'
        'master/source/_static/css/fiware_readthedocs.css')
    try:
        req = requests.get(url, allow_redirects=True, timeout=10)
    except requests.RequestException as e:
        print(
            'Failed to download the CSS with the eProsima rtd theme.'
            'Request Error: {}'.format(e)
        )
        return False
    if req.status_code != 200:
        print(
            'Failed to download the CSS with the eProsima rtd theme.'
            'Return code: {}'.format(req.status_code))
        return False
    os.makedirs(
        os.path.dirname('{}/_static/css/'.format(html_css_dir)),
        exist_ok=True)
    theme_path = '{}/_static/css/online_eprosima_rtd_theme.css'.format(
        html_css_dir)
    with open(theme_path, 'wb') as f:
        try:
            f.write(req.content)
        except OSError:
            print('Failed to create the file: {}'.format(theme_path))
            return False
    return True


def select_css(html_css_dir):
    """
    Select CSS file with the website's template.

    :param html_css_dir: The directory to save the CSS stylesheet.
    :return: Returns a list of CSS files to be imported.
    """
    ret = ['_static/tabs.css']
    common_css = '_static/css/online_eprosima_rtd_theme.css'
    local_css = '_static/css/eprosima_rtd_theme.css'
    if download_css(html_css_dir):
        print('Applying common CSS style file: {}'.format(common_css))
        ret.append(common_css)
    else:
        print('Applying local CSS style file: {}'.format(local_css))
        ret.append(local_css)

    return ret


script_path = os.path.abspath(pathlib.Path(__file__).parent.absolute())
# Project directories
project_source_docs_dir = os.path.abspath('{}/rst'.format(script_path))
project_binary_dir = os.path.abspath('{}/../build'.format(script_path))
project_binary_docs_dir = os.path.abspath(
    '{}/docs'.format(project_binary_dir)
)

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.todo',
    'sphinx_tabs.tabs'
]

sphinx_tabs_disable_css_loading = False
sphinx_tabs_disable_tab_closing = True

try:
    import sphinxcontrib.spelling  # noqa: F401
    extensions.append('sphinxcontrib.spelling')

    # spelling_word_list_filename = 'spelling_wordlist.txt'
    spelling_word_list_filename = [
        'rst/spelling_wordlist.txt',
    ]

    from sphinxcontrib.spelling.filters import ContractionFilter
    spelling_filters = [ContractionFilter]
    spelling_ignore_contributor_names = False
except ImportError:
    pass

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = [
    '*/includes/*.rst',
    '*/*/includes/*.rst',
    '*/*/*/includes/*.rst',
    '*/*/*/*/includes/*.rst'
]

# The suffix(es) of source filenames.
# You can specify multiple suffix as a list of string:
#
# source_suffix = ['.rst', '.md']
source_suffix = '.rst'

# The encoding of source files.
#
# source_encoding = 'utf-8-sig'

# The master toctree document.
master_doc = 'index'

# -- Project information -----------------------------------------------------

project = PROJECT_NAME
copyright = '2023, eProsima'
author = 'eProsima'

# The version info for the project you're documenting, acts as replacement for
# |version| and |release|, also used in various other places throughout the
# built documents.
#
# The short X.Y version.
versions = get_version('{}/VERSION'.format(script_path))
if versions is None:
    versions = get_version(
        os.path.abspath('{}/../VERSION'.format(script_path)))
if versions is None:
    versions = get_version_from_cmakelists(
        os.path.abspath('{}/CMakeLists.txt'.format(script_path)))

version = u'{}.{}'.format(versions['major'], versions['minor'])
# The full version, including alpha/beta/rc tags.
release = u'{}.{}.{}'.format(
    versions['major'], versions['minor'], versions['patch'])

# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#
# This is also used if you do content translation via gettext catalogs.
# Usually you set "language" from the command line for these cases.
language = "en"

# There are two options for replacing |today|: either, you set today to some
# non-false value, then it is used:
#
# today = ''
#
# Else, today_fmt is used as the format for a strftime call.
#
# today_fmt = '%B %d, %Y'

# The reST default role (used for this markup: `text`) to use for all
# documents.
#
# default_role = None

# If true, '()' will be appended to :func: etc. cross-reference text.
#
# add_function_parentheses = True

# If true, the current module name will be prepended to all description
# unit titles (such as .. function::).
#
# add_module_names = True

# If true, sectionauthor and moduleauthor directives will be shown in the
# output. They are ignored by default.
#
# show_authors = False

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'sphinx'

# A list of ignored prefixes for module index sorting.
# modindex_common_prefix = []

# If true, keep warnings as "system message" paragraphs in the built documents.
# keep_warnings = False

suppress_warnings = []

# If true, `todo` and `todoList` produce output, else they produce nothing.
todo_include_todos = False


# -- Options for HTML output ----------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Theme options are theme-specific and customize the look and feel of a theme
# further.  For a list of options available for each theme, see the
# documentation.
#
# html_theme_options = {}

# Add any paths that contain custom themes here, relative to this directory.
# html_theme_path = []

# The name for this set of Sphinx documents.
# "<project> v<release> documentation" by default.
#
# html_title = u'sphynx-demo v0.0.1'

# A shorter title for the navigation bar.  Default is the same as html_title.
#
# html_short_title = None

# The name of an image file (relative to this directory) to place at the top
# of the sidebar.
#
# html_logo = None

# The name of an image file (relative to this directory) to use as a favicon of
# the docs. This file should be a Windows icon file (.ico) being 16x16 or 32x32
# pixels large.

#html_favicon = 'rst/_static/css/imgs/eProsima.ico'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['rst/_static']

html_context = {
        'css_files': select_css(project_source_docs_dir),
        }


# Add any extra paths that contain custom files (such as robots.txt or
# .htaccess) here, relative to this directory. These files are copied
# directly to the root of the documentation.
#
# html_extra_path = []

# If not None, a 'Last updated on:' timestamp is inserted at every page
# bottom, using the given strftime format.
# The empty string is equivalent to '%b %d, %Y'.
#
# html_last_updated_fmt = None

# If true, SmartyPants will be used to convert quotes and dashes to
# typographically correct entities.
#
# html_use_smartypants = True

# Custom sidebar templates, maps document names to template names.
#
# html_sidebars = {}

# Additional templates that should be rendered to pages, maps page names to
# template names.
#
# html_additional_pages = {}

# If false, no module index is generated.
#
# html_domain_indices = True

# If false, no index is generated.
#
# html_use_index = True

# If true, the index is split into individual pages for each letter.
#
# html_split_index = False

# If true, links to the reST sources are added to the pages.
#
# html_show_sourcelink = True

# If true, "Created using Sphinx" is shown in the HTML footer. Default is True.

html_show_sphinx = False

# If true, "(C) Copyright ..." is shown in the HTML footer. Default is True.
#
# html_show_copyright = True

# If true, an OpenSearch description file will be output, and all pages will
# contain a <link> tag referring to it.  The value of this option must be the
# base URL from which the finished HTML is served.
#
# html_use_opensearch = ''

# This is the file name suffix for HTML files (e.g. ".xhtml").
# html_file_suffix = None

# Language to be used for generating the HTML full-text search index.
# Sphinx supports the following languages:
#   'da', 'de', 'en', 'es', 'fi', 'fr', 'hu', 'it', 'ja'
#   'nl', 'no', 'pt', 'ro', 'ru', 'sv', 'tr', 'zh'
#
# html_search_language = 'en'

# A dictionary with options for the search language support, empty by default.
# 'ja' uses this config value.
# 'zh' user can custom change `jieba` dictionary path.
#
# html_search_options = {'type': 'default'}

# The name of a javascript file (relative to the configuration directory) that
# implements a search results scorer. If empty, the default will be used.
#
# html_search_scorer = 'scorer.js'

# Output file base name for HTML help builder.
htmlhelp_basename = f'{PROJECT_NAME} Manual'

# -- Options for LaTeX output ---------------------------------------------

latex_elements = {
     # The paper size ('letterpaper' or 'a4paper').
     #
     # 'papersize': 'letterpaper',

     # The font size ('10pt', '11pt' or '12pt').
     #
     # 'pointsize': '10pt',

     # Additional stuff for the LaTeX preamble.
     #
     # 'preamble': '',

     # Latex figure (float) alignment
     #
     # 'figure_align': 'htbp',
}

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    (master_doc,
     f'{COMPRESS_PROJECT_NAME}.tex',
     f'{PROJECT_NAME} Documentation',
     'eProsima',
     'manual'),
]

# The name of an image file (relative to this directory) to place at the top of
# the title page.
#
# latex_logo = 01-figures/logo.png

# For "manual" documents, if this is true, then toplevel headings are parts,
# not chapters.
#
# latex_use_parts = False

# If true, show page references after internal links.
#
# latex_show_pagerefs = False

# If true, show URL addresses after external links.
#
# latex_show_urls = False

# Documents to append as an appendix to all manuals.
#
# latex_appendices = []

# It false, will not define \strong, \code, 	itleref, \crossref ... but only
# \sphinxstrong, ..., \sphinxtitleref, ... To help avoid clash with user added
# packages.
#
# latex_keep_old_macro_names = True

# If false, no module index is generated.
#
# latex_domain_indices = True


# -- Options for manual page output ---------------------------------------

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    (master_doc,
     PROJECT_NAME,
     f'{PROJECT_NAME} Documentation',
     [author],
     1)
]

# If true, show URL addresses after external links.
#
# man_show_urls = False


# -- Options for Texinfo output -------------------------------------------

# Grouping the document tree into Texinfo files. List of tuples
# (source start file, target name, title, author,
#  dir menu entry, description, category)
texinfo_documents = [
    (master_doc,
     PROJECT_NAME,
     f'{PROJECT_NAME} Documentation',
     author,
     PROJECT_NAME,
     f'Documentation of eProsima {PROJECT_NAME}',
     'Miscellaneous'),
]

# Documents to append as an appendix to all manuals.
#
# texinfo_appendices = []

# If false, no module index is generated.
#
# texinfo_domain_indices = True

# How to display URL addresses: 'footnote', 'no', or 'inline'.
#
# texinfo_show_urls = 'footnote'

# If true, do not generate a @detailmenu in the "Top" node's menu.
#
# texinfo_no_detailmenu = False

# Manually inserted by eProsima
# Enable the figure referencing
numfig = True
