[![Build and tests](https://github.com/jd-boyd/python-lzo/actions/workflows/wheels.yml/badge.svg)](https://github.com/jd-boyd/python-lzo/actions/workflows/wheels.yml)

```
                 Python-LZO -- Python bindings for LZO

              Copyright (c) 1996-2002 Markus F.X.J. Oberhumer
                          <markus@oberhumer.com>
                 http://www.oberhumer.com/opensource/lzo/
              Copyright (c) 2011-2016 Joshua D. Boyd
                          <jdboyd@jdboyd.net>
                 https://github.com/jd-boyd/python-lzo
```

# What is LZO ?


LZO is a portable lossless data compression library written in ANSI C.
It offers pretty fast compression and *very* fast decompression.
Decompression requires no memory.

In addition there are slower compression levels achieving a quite
competitive compression ratio while still decompressing at
this very high speed.


# What is Python-LZO ?

Python-LZO provides Python bindings for LZO, i.e. you can access
the LZO library from your Python scripts thereby compressing ordinary
Python strings.


# Installation

## Pre-reqs

On linux, you will want to install `liblzo2-dev` or your distributions equivelent.

```bash
sudo apt install liblzo2-dev
```
or
```bash
sudo yum install liblzo2-devel
```
and the two most likely ways.

## Actual Install

```
pip install python-lzo
```
Or explicitly from source,
either from a specific release or from the repo (requires build tools):
```
pip install python-lzo-x.y.tar.gz
pip install https://[...]/python-lzo-x.y.tar.gz
pip install git+https://github.com/jd-boyd/python-lzo
```

# Building from source

Building from source requires build tools. On most Linux distributions
they are probably already installed. On Windows you need
[Microsoft C++ Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/)
(which should already be installed if you have Visual Studio).
On macOS you need XCode installed, or something else that provides a suitable C
compiler. Then either `git clone`, or download a source distribution and untar it.
Once you are in the root of the project directory where `pyproject.toml` is located,
run `python -m build -w`. This should build a wheel in the `dist` directory.
You might need to install `build` with `pip install build`.

If you really want to build a wheel for Python 2.7 on Windows you'll need the
[Microsoft Visual C++ Compiler for Python 2.7](https://web.archive.org/web/20210116063838/https://www.microsoft.com/en-us/download/details.aspx?id=44266).

# Where's the documentation ?

Python-LZO comes with built-in documentation which is accessible using
```py
>>> import lzo
>>> help(lzo)
```
Additionally you should read the docs and study the example
programs that ship with the LZO library.

# Python 2 support statement

Python 2.7 is still supported but without being a priority.
Support will be dropped soon.

# Notes

Wheels are built with [cibuildwheel](https://cibuildwheel.readthedocs.io/)
on GitHub Actions. Tests are run for all combinations of platform and
Python version that it can run tests for.

# Releasing

1. Update version in `pyproject.toml`, `setup.py` and the `MODULE_VERSION`
    define in `lzomodule.c`.
1. Update NEWS.
1. Tag with new release.
1. wheels (download from github actions)
1. Upload to PyPi (`twine upload dist/*`)

# Copyright

The LZO and Python-LZO algorithms and implementations are
Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002
Markus Franz Xaver Johannes Oberhumer <markus@oberhumer.com>

The Python-LZO algorithms implementated post 2011 are
Copyright (C) 2011, 2014, 2015, 2016, 2017, 2018, 2019, 2020, 2021,
2022, & 2023
Joshua D. Boyd <jdboyd@jdboyd.net> and others as denoted in the git
history.


The LZO and Python-LZO algorithms and implementations are distributed under
the terms of the GNU General Public License (GPL).  See the file COPYING.
