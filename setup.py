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
)
