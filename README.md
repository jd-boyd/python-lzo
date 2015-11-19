![TravisCI Status](https://travis-ci.org/jd-boyd/python-lzo.png)

```
                 Python-LZO -- Python bindings for LZO

              Copyright (c) 1996-2002 Markus F.X.J. Oberhumer
                          <markus@oberhumer.com>
                 http://www.oberhumer.com/opensource/lzo/
              Copyright (c) 2011-2014 Joshua D. Boyd
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


# What is Python ?

Python is an interpreted object-oriented programming language, and is
often compared to Tcl, Perl, Java or Scheme. To find out more, point
your browser to http://www.python.org/ .


# What is Python-LZO ?

Python-LZO provides Python bindings for LZO, i.e. you can access
the LZO library from your Python scripts thereby compressing ordinary
Python strings.


# Where's the documentation ?

Python-LZO comes with built-in documentation which is accessible
using `lzo.__doc__' and `lzo.func__doc__'. See "Chapter 3: Data Model"
in the Python Reference Manual for more information.

Additionally you should read the docs and study the example
programs that ship with the LZO library. Really.

# Notes

The windows version is untested.  If you do test it, please let me
know the results.

# Copyright

The LZO and Python-LZO algorithms and implementations are
Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002
Markus Franz Xaver Johannes Oberhumer <markus@oberhumer.com>

The Python-LZO algorithms implementated post 2011 are
Copyright (C) 2011, 2014
Joshua D. Boyd <jdboyd@jdboyd.net>

The LZO and Python-LZO algorithms and implementations are distributed under
the terms of the GNU General Public License (GPL).  See the file COPYING.
