#!/usr/bin/env python
# vi:ts=4:et

import os
import re
import shutil
import subprocess
import sys
import tempfile
from glob import glob
from setuptools import Command, Extension, setup


def _get_version():
    with open("pyproject.toml", "r") as f:
        for line in f:
            m = re.match(r'^version\s*=\s*"([^"]+)"', line)
            if m:
                return m.group(1)
    return "unknown"


_package_version = _get_version()


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


def has_system_lzo2():
    """Return True if liblzo2 can be compiled and linked against."""
    if sys.platform == "win32":
        return False
    test_c = """
    #include <lzo/lzo1x.h>
    int main(void) { return lzo_init() == LZO_E_OK ? 0 : 1; }
    """
    tmpdir = tempfile.mkdtemp()
    try:
        src = os.path.join(tmpdir, "test.c")
        out = os.path.join(tmpdir, "test")
        with open(src, "w") as f:
            f.write(test_c)
        cc = os.environ.get("CC", "cc")
        result = subprocess.run(
            [cc, "-o", out, src, "-llzo2"],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
        return result.returncode == 0
    finally:
        shutil.rmtree(tmpdir)


lzo_dir = os.environ.get("LZO_DIR", "lzo-2.10")  # Relative path.

_use_system_lzo = has_system_lzo2()

src_list = ["lzomodule.c"]
if not _use_system_lzo:
    src_list += glob(os.path.join(lzo_dir, "src/*.c"))

setup(
    cmdclass={
        "test": TestCommand
    },
    ext_modules=[
        Extension(
            name="lzo",
            sources=src_list,
            include_dirs=[os.path.join(lzo_dir, "include")],
            libraries=['lzo2'] if _use_system_lzo else [],
            library_dirs=[os.path.join(lzo_dir, "lib")],
            define_macros=[("MODULE_VERSION", '"%s"' % _package_version)],
            #extra_link_args=["-flat_namespace"] if sys.platform == "darwin" else [],
        )
    ],
)
