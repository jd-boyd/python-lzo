#!/usr/bin/env python
# vi:ts=4:et

from __future__ import print_function

import os
import subprocess
import sys
from glob import glob
from setuptools import Command, Extension, setup


class TestCommand(Command):
    user_options = []

    def initialize_options(self):
        pass

    def finalize_options(self):
        pass

    def run(self):
        raise SystemExit(
            subprocess.call([sys.executable,
                             "-m",
                             "pytest"]))


lzo_dir = os.environ.get("LZO_DIR", "lzo-2.10")  # Relative path.

setup(
    name="python-lzo",
    version="1.15",
    description="Python bindings for the LZO data compression library",
    author="Markus F.X.J. Oberhumer",
    author_email="markus@oberhumer.com",
    maintainer="Joshua D. Boyd",
    maintainer_email="jdboyd@jdboyd.net",
    url="https://github.com/jd-boyd/python-lzo",
    license="GNU General Public License (GPL)",
    tests_require=['pytest'],
    cmdclass={
        "test": TestCommand
    },
    ext_modules=[
        Extension(
            name="lzo",
            sources=["lzomodule.c"] + glob(os.path.join(lzo_dir, "src/*.c")),
            include_dirs=[os.path.join(lzo_dir, "include")],
            extra_link_args=["-flat_namespace"] if sys.platform == "darwin" else [],
        )
    ],
    long_description="""
This module provides Python bindings for the LZO data compression library.
LZO is a portable lossless data compression library written in ANSI C.
It offers pretty fast compression and *very* fast decompression.
Decompression requires no memory.
In addition there are slower compression levels achieving a quite
competitive compression ratio while still decompressing at
this very high speed.""",
)
