![TravisCI Status](https://travis-ci.org/jd-boyd/python-lzo.png)
[![Appveyor Build Status](https://ci.appveyor.com/api/projects/status/github/jd-boyd/python-lzo?svg=true)](https://ci.appveyor.com/project/jd-boyd/python-lzo/branch/master)

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

## Ubuntu Linux

You need the following dependencies installed:
* `zlib1g-dev`
* `liblzo2-dev`
* `python-pip` or `python3-pip`

Then, just `pip install python-lzo`.

These directions are probably the same as debian.  The requirements
are similar for Redhat/centos/etc, but the specific package names
differ.

## macOS

You need XCode installed, or something else that provides suitable C
compiler.  Then just:

`pip install python-lzo`

## Windows

There is an issue filed to provide a pre-compiled wheel to greatly
simplify this.

Until then, here we go:
* Download http://www.oberhumer.com/opensource/lzo/download/lzo-2.09.tar.gz
* extract that file to `c:\src\` (should produce `c:\src\lzo-2.09` )
* Install a visual studio compatible with your python build.  If you are using
  python 2.7, MS has a package just for you at: https://www.microsoft.com/en-us/download/details.aspx?id=44266
  Current automated tests are done with Visual Studio 2013, but newer should work as well.
* `python.exe setup.py install`

# Where's the documentation ?

Python-LZO comes with built-in documentation which is accessible
using `lzo.__doc__` and `lzo.func__doc__`. See ["Chapter 3: Data Model"
in the Python Reference Manual](https://docs.python.org/3.6/reference/datamodel.html) for more information.

Additionally you should read the docs and study the example
programs that ship with the LZO library. Really.

# Notes

The Windows version is tested.

It is not currently tested on OSX, but that is coming.

# Copyright

The LZO and Python-LZO algorithms and implementations are
Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002
Markus Franz Xaver Johannes Oberhumer <markus@oberhumer.com>

The Python-LZO algorithms implementated post 2011 are
Copyright (C) 2011, 2014, 2015, 2016, 2017, 2018
Joshua D. Boyd <jdboyd@jdboyd.net>

The LZO and Python-LZO algorithms and implementations are distributed under
the terms of the GNU General Public License (GPL).  See the file COPYING.
