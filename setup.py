#! /usr/bin/env python
# vi:ts=4:et

from __future__ import print_function

import os, sys
import distutils
from distutils.cmd import Command
from distutils.core import setup
from distutils.extension import Extension
from distutils.util import split_quoted

include_dirs = []
define_macros = []
library_dirs = []
libraries = []
runtime_library_dirs = []
extra_objects = []
extra_compile_args = []
extra_link_args = []

class TestCommand(Command):
    user_options = []

    def initialize_options(self):
        pass

    def finalize_options(self):
        pass

    def run(self):
        import sys, subprocess
        raise SystemExit(
            subprocess.call([sys.executable,
                             '-m',
                             'nose']))


if sys.platform == "win32":
    # Windows users have to configure the LZO_DIR path parameter to match
    # their LZO source installation.  The path set here is just an example
    # and thus unlikely to match your installation.

    LZO_DIR = os.environ.get('LZO_DIR', r"c:\src\lzo-1.08")
    if not os.path.exists(LZO_DIR):
        raise Exception("please set LZO_DIR to where the lzo source lives")
    include_dirs.append(os.path.join(LZO_DIR, r"include\lzo"))
    include_dirs.append(os.path.join(LZO_DIR, "include"))
    lib1_file = os.path.join(LZO_DIR, "lzo.lib")
    lib2_file = os.path.join(LZO_DIR, "lzo2.lib")
    if os.path.exists(lib2_file):
        lib_file = lib2_file
    else:
        lib_file = lib1_file
    extra_objects.append(lib_file)
else:
    libraries = ["lzo2"]
    include_dirs.append(os.environ.get("PREFIX", "/usr")+"/include/lzo")
    ##library_dirs.append("/usr/local/lib")
    ##runtime_library_dirs.append("/usr/local/lib")

    # Add extra compile flag for MacOS X
    if sys.platform[:-1] == "darwin":
        extra_link_args.append("-flat_namespace")


###############################################################################

def get_kw(**kw): return kw

ext = Extension(
    name="lzo",
    sources=["lzomodule.c"],
    include_dirs=include_dirs,
    define_macros=define_macros,
    library_dirs=library_dirs,
    libraries=libraries,
    runtime_library_dirs=runtime_library_dirs,
    extra_objects=extra_objects,
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
)

setup_args = get_kw(
    name="python-lzo",
    version="1.12",
    description="Python bindings for the LZO data compression library",
    author="Markus F.X.J. Oberhumer",
    author_email="markus@oberhumer.com",
    maintainer="Joshua D. Boyd",
    maintainer_email="jdboyd@jdboyd.net",
    url="https://github.com/jd-boyd/python-lzo",
    license="GNU General Public License (GPL)",
    tests_require=['nose'],
    cmdclass={
        'test': TestCommand
    },
    ext_modules=[ext],
    long_description="""
This module provides Python bindings for the LZO data compression library.

LZO is a portable lossless data compression library written in ANSI C.
It offers pretty fast compression and *very* fast decompression.
Decompression requires no memory.

In addition there are slower compression levels achieving a quite
competitive compression ratio while still decompressing at
this very high speed.""",
)

if distutils.__version__ >= "1.0.2":
    setup_args["platforms"] = "All"

setup(**setup_args)
