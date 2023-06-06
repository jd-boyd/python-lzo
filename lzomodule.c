/* lzomodule.c -- Python bindings for the LZO compression library

   This file is part of the LZO real-time data compression library.

   Copyright (C) 2002 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2001 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2000 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1999 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1998 Markus Franz Xaver Johannes Oberhumer

   The LZO library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The LZO library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the LZO library; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Markus F.X.J. Oberhumer
   <markus@oberhumer.com>
   http://www.oberhumer.com/opensource/lzo/
 */


#define MODULE_VERSION  "1.15"

#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <string.h>
#include <lzo/lzo1.h>
#include <lzo/lzo1a.h>
#include <lzo/lzo1b.h>
#include <lzo/lzo1c.h>
#include <lzo/lzo1f.h>
#include <lzo/lzo1x.h>
#include <lzo/lzo1y.h>
#include <lzo/lzo1z.h>
#include <lzo/lzo2a.h>

/* Python 2x3 compatible macros */
#if PY_VERSION_HEX >= 0x03000000
#  define PyInt_FromLong PyLong_FromLong
#  define PyString_FromString PyBytes_FromString
#  define PyString_FromStringAndSize PyBytes_FromStringAndSize
#  define PyString_AsString PyBytes_AsString
#  define _PyString_Resize _PyBytes_Resize
#endif

/* Ensure we have updated versions */
#if !defined(PY_VERSION_HEX) || (PY_VERSION_HEX < 0x010502f0)
#  error "Need Python version 1.5.2 or greater"
#endif
#if !defined(LZO_VERSION) || (LZO_VERSION < 0x1070)
#  error "Need LZO version 1.07 or greater"
#endif




#undef UNUSED
#define UNUSED(var)     ((void)&var)

static PyObject *LzoError;

// custom function type definitions to allow compatibility of various algorithms
typedef int (*lzo_compress_fn)(const lzo_bytep, lzo_uint, lzo_bytep, lzo_uintp, lzo_voidp);
typedef int (*lzo_decompress_fn)(const lzo_bytep, lzo_uint, lzo_bytep, lzo_uintp, lzo_voidp /* NOT USED */);

/***********************************************************************
// compress
************************************************************************/

static /* const */ char compress__doc__[] =
"compress(string[,level[,header[,algorithm]]]) -- Compress string, returning a string "
"containing compressed data.\n"
"level  - Set compression level of either 1 (default) or 9.\n"
"header - Include metadata header for decompression in the output "
"(default: True).\n"
"algorithm (keyword argument)  - can be either LZO1, LZO1A, LZO1B, LZO1C, LZO1F, LZO1X, LZO1Y, LZO1Z, LZO2A."
"(default: LZO1X).\n"
;

static PyObject *
compress(PyObject *dummy, PyObject *args, PyObject *kwds)
{
    PyObject *result_str;
    lzo_voidp wrkmem = NULL;
    const lzo_bytep in;
    lzo_bytep out;
    lzo_bytep outc;
    lzo_uint in_len;
    lzo_uint out_len;
    lzo_uint new_len;
    Py_ssize_t len;
    int level = 1;
    int header = 1;
    int err;

    static char* argnames[] = {"", "", "", "algorithm", NULL};
    char *algorithm = "LZO1X";
    lzo_compress_fn compress_1_ptr;
    lzo_compress_fn compress_999_ptr;
    lzo_uint32_t MEM_COMPRESS_1;
    lzo_uint32_t MEM_COMPRESS_999;

    /* init */
    UNUSED(dummy);
#if PY_VERSION_HEX >= 0x03030000
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#|ii$s", argnames, &in, &len, &level, &header, &algorithm))
#else
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#|iis", argnames, &in, &len, &level, &header, &algorithm))
#endif
        return NULL;
    if (len < 0)
        return NULL;

    if (len > LZO_UINT_MAX) {
      PyErr_SetString(LzoError, "Input size is larger than LZO_UINT_MAX");
      return NULL;
    }

    if ((len + len / 16 + 64 + 3) > LZO_UINT_MAX) {
      PyErr_SetString(LzoError, "Output size is larger than LZO_UINT_MAX");
      return NULL;
    }

    /* init for different algorithms*/
    if (strcmp(algorithm, "LZO1") == 0) {
      // settings specific for lzo1
      MEM_COMPRESS_1 = LZO1_MEM_COMPRESS;
      MEM_COMPRESS_999 = LZO1_99_MEM_COMPRESS;

      compress_1_ptr = &lzo1_compress;
      compress_999_ptr = &lzo1_99_compress;
    }
    else if (strcmp(algorithm, "LZO1A") == 0){
      // settings for LZO1A
      MEM_COMPRESS_1 = LZO1A_MEM_COMPRESS;
      MEM_COMPRESS_999 = LZO1A_99_MEM_COMPRESS;

      compress_1_ptr = &lzo1a_compress;
      compress_999_ptr = &lzo1a_99_compress;
    }
    else if (strcmp(algorithm, "LZO1B") == 0){
      // settings for LZO1B
      MEM_COMPRESS_1 = LZO1B_MEM_COMPRESS;
      MEM_COMPRESS_999 = LZO1B_999_MEM_COMPRESS;

      compress_1_ptr = &lzo1b_1_compress;
      compress_999_ptr = &lzo1b_999_compress;
    }
    else if (strcmp(algorithm, "LZO1C") == 0){
      // settings for LZO1C
      MEM_COMPRESS_1 = LZO1C_MEM_COMPRESS;
      MEM_COMPRESS_999 = LZO1C_999_MEM_COMPRESS;

      compress_1_ptr = &lzo1c_1_compress;
      compress_999_ptr = &lzo1c_999_compress;
    }
    else if (strcmp(algorithm, "LZO1F") == 0){
      // settings for LZO1F
      MEM_COMPRESS_1 = LZO1F_MEM_COMPRESS;
      MEM_COMPRESS_999 = LZO1F_999_MEM_COMPRESS;

      compress_1_ptr = &lzo1f_1_compress;
      compress_999_ptr = &lzo1f_999_compress;
    }
    else if (strcmp(algorithm, "LZO1Y") == 0){
      // settings for LZO1Y
      MEM_COMPRESS_1 = LZO1Y_MEM_COMPRESS;
      MEM_COMPRESS_999 = LZO1Y_999_MEM_COMPRESS;

      compress_1_ptr = &lzo1y_1_compress;
      compress_999_ptr = &lzo1y_999_compress;
    }
    else if (strcmp(algorithm, "LZO1Z") == 0){
      // settings for LZO1Z
      MEM_COMPRESS_1 = LZO1Z_999_MEM_COMPRESS;
      MEM_COMPRESS_999 = LZO1Z_999_MEM_COMPRESS;

      compress_1_ptr = &lzo1z_999_compress;
      compress_999_ptr = &lzo1z_999_compress;
    }
    else if (strcmp(algorithm, "LZO2A") == 0){
      // settings for LZO2A
      MEM_COMPRESS_1 = LZO2A_999_MEM_COMPRESS;
      MEM_COMPRESS_999 = LZO2A_999_MEM_COMPRESS;

      compress_1_ptr = &lzo2a_999_compress;
      compress_999_ptr = &lzo2a_999_compress;
    }
    else {
      // settings for LZO1X
      MEM_COMPRESS_1 = LZO1X_1_MEM_COMPRESS;
      MEM_COMPRESS_999 = LZO1X_999_MEM_COMPRESS;

      compress_1_ptr = &lzo1x_1_compress;
      compress_999_ptr = &lzo1x_999_compress;
    }

    in_len = len;
    out_len = in_len + in_len / 16 + 64 + 3;

    /* alloc buffers */
#if PY_MAJOR_VERSION >= 3
    result_str = PyBytes_FromStringAndSize(NULL, 5 + out_len);
#else
    result_str = PyString_FromStringAndSize(NULL, 5 + out_len);
#endif
    if (result_str == NULL)
        return PyErr_NoMemory();
    if (level == 1)
        wrkmem = (lzo_voidp) PyMem_Malloc(MEM_COMPRESS_1);
    else
        wrkmem = (lzo_voidp) PyMem_Malloc(MEM_COMPRESS_999);
    if (wrkmem == NULL)
    {
        Py_DECREF(result_str);
        return PyErr_NoMemory();
    }

    /* compress */
#if PY_MAJOR_VERSION >= 3
    out = (lzo_bytep) PyBytes_AsString(result_str);
#else
    out = (lzo_bytep) PyString_AsString(result_str);
#endif

    Py_BEGIN_ALLOW_THREADS
    outc = header ? out+5 : out; // leave space for header if needed
    new_len = out_len;
    if (level == 1)
    {
        if (header)
            out[0] = 0xf0;
        err = (*compress_1_ptr)(in, in_len, outc, &new_len, wrkmem);
    }
    else
    {
        if (header)
            out[0] = 0xf1;
        err = (*compress_999_ptr)(in, in_len, outc, &new_len, wrkmem);
    }
    Py_END_ALLOW_THREADS

    PyMem_Free(wrkmem);
    if (err != LZO_E_OK || new_len > out_len)
    {
        /* this should NEVER happen */
        Py_DECREF(result_str);
        PyErr_Format(LzoError, "Error %i while compressing data", err);
        return NULL;
    }

    if (header) {
        /* save uncompressed length */
        out[1] = (unsigned char) ((in_len >> 24) & 0xff);
        out[2] = (unsigned char) ((in_len >> 16) & 0xff);
        out[3] = (unsigned char) ((in_len >>  8) & 0xff);
        out[4] = (unsigned char) ((in_len >>  0) & 0xff);
    }

    /* return */
    if (new_len != out_len)
#if PY_MAJOR_VERSION >= 3
        _PyBytes_Resize(&result_str, header ? new_len + 5 : new_len);
#else
        _PyString_Resize(&result_str, header ? new_len + 5 : new_len);
#endif

    return result_str;
}


/***********************************************************************
// decompress
************************************************************************/

static /* const */ char decompress__doc__[] =
"decompress(string[,header[,buflen[,algorithm]]]) -- Decompress the data in string, returning a string containing the decompressed data.\n"
"header - Metadata header is included in input (default: True).\n"
"buflen - If header is False, a buffer length in bytes must be given that "
"will fit the output.\n"
"algorithm (keyword argument) - can be either LZO1, LZO1A, LZO1B, LZO1C, LZO1F, LZO1X, LZO1Y, LZO1Z, LZO2A."
"(default: LZO1X).\n"
;

static PyObject *
decompress(PyObject *dummy, PyObject *args, PyObject *kwds)
{
    PyObject *result_str;
    const lzo_bytep in;
    lzo_bytep out;
    lzo_uint in_len;
    lzo_uint out_len;
    lzo_uint new_len;
    Py_ssize_t len;
    int buflen = -1;
    int header = 1;
    int err;

    static char* argnames[] = {"", "", "", "algorithm", NULL};
    char *algorithm = "LZO1X";
    lzo_decompress_fn decompress_ptr;

    /* init */
    UNUSED(dummy);
#if PY_VERSION_HEX >= 0x03030000
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#|ii$s", argnames, &in, &len, &header, &buflen, &algorithm))
#else
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#|iis", argnames, &in, &len, &header, &buflen, &algorithm))
#endif
        return NULL;
    if (header) {
        if (len < 5 + 3 || in[0] < 0xf0 || in[0] > 0xf1)
            goto header_error;
        out_len = ((lzo_uint)in[1] << 24) | (in[2] << 16) | (in[3] << 8) | in[4];
        in_len = len - 5;
        in += 5;
        if (out_len < 0 || in_len > out_len + out_len / 64 + 16 + 3)
            goto header_error;
    }
    else {
        if (buflen < 0) return PyErr_Format(LzoError, "Argument buflen required for headerless decompression");
        out_len = buflen;
        in_len = len;
    }

    /* init for different algorithms*/
    if (strcmp(algorithm, "LZO1") == 0) {
      // settings specific for lzo1
      decompress_ptr = &lzo1_decompress;
    }
    else if (strcmp(algorithm, "LZO1A") == 0){
      // settings for LZO1A
      decompress_ptr = &lzo1a_decompress;
    }
    else if (strcmp(algorithm, "LZO1B") == 0){
      // settings for LZO1B
      decompress_ptr = &lzo1b_decompress_safe;
    }
    else if (strcmp(algorithm, "LZO1C") == 0){
      // settings for LZO1C
      decompress_ptr = &lzo1c_decompress_safe;
    }
    else if (strcmp(algorithm, "LZO1F") == 0){
      // settings for LZO1F
      decompress_ptr = &lzo1f_decompress_safe;
    }
    else if (strcmp(algorithm, "LZO1Y") == 0){
      // settings for LZO1Y
      decompress_ptr = &lzo1y_decompress_safe;
    }
    else if (strcmp(algorithm, "LZO1Z") == 0){
      // settings for LZO1Z
      decompress_ptr = &lzo1z_decompress_safe;
    }
    else if (strcmp(algorithm, "LZO2A") == 0){
      // settings for LZO2A
      decompress_ptr = &lzo2a_decompress_safe;
    }
    else {
      // settings for LZO1X
      decompress_ptr = &lzo1x_decompress_safe;
    }

    /* alloc buffers */
#if PY_MAJOR_VERSION >= 3
    result_str = PyBytes_FromStringAndSize(NULL, out_len);
#else
    result_str = PyString_FromStringAndSize(NULL, out_len);
#endif
    if (result_str == NULL)
        return PyErr_NoMemory();

    /* decompress */
#if PY_MAJOR_VERSION >= 3
    out = (lzo_bytep) PyBytes_AsString(result_str);
#else
    out = (lzo_bytep) PyString_AsString(result_str);
#endif

    Py_BEGIN_ALLOW_THREADS
    new_len = out_len;
    err = (*decompress_ptr)(in, in_len, out, &new_len, NULL);
    Py_END_ALLOW_THREADS

    if (err != LZO_E_OK || (header && new_len != out_len) )
    {
        Py_DECREF(result_str);
        PyErr_Format(LzoError, "Compressed data violation %i", err);
        return NULL;
    }

    if (!header && new_len != out_len)
        _PyString_Resize(&result_str, new_len);

    /* success */
    return result_str;

header_error:
    PyErr_SetString(LzoError, "Header error - invalid compressed data");
    return NULL;
}


/***********************************************************************
// optimize
************************************************************************/

static /* const */ char optimize__doc__[] =
"optimize(string[,header[,buflen]]) -- Optimize the representation of the "
"compressed data, returning a string containing the compressed data.\n"
"header - Metadata header is included in input (default: True).\n"
"buflen - If header is False, a buffer length in bytes must be given that "
"will fit the input/output.\n"
;

static PyObject *
optimize(PyObject *dummy, PyObject *args)
{
    PyObject *result_str;
    lzo_bytep in;
    lzo_bytep out;
    lzo_uint in_len;
    lzo_uint out_len;
    lzo_uint new_len;
    Py_ssize_t len;
    int err;
    int header = 1;
    int buflen = -1;

    /* init */
    UNUSED(dummy);
    if (!PyArg_ParseTuple(args, "s#|ii", &in, &len, &header, &buflen))
        return NULL;
    if (header) {
        if (len < 5 + 3 || in[0] < 0xf0 || in[0] > 0xf1)
            goto header_error;
        in_len = len - 5;
        out_len = (in[1] << 24) | (in[2] << 16) | (in[3] << 8) | in[4];
        if ((int)out_len < 0 || in_len > out_len + out_len / 64 + 16 + 3)
            goto header_error;
    }
    else {
        if (buflen < 0) return PyErr_Format(LzoError, "Argument buflen required for headerless optimization");
        out_len = buflen;
        in_len = len;
    }

    /* alloc buffers */
#if PY_MAJOR_VERSION >= 3
    result_str = PyBytes_FromStringAndSize(in, len);
#else
    result_str = PyString_FromStringAndSize(in, len);
#endif
    if (result_str == NULL)
        return PyErr_NoMemory();
    out = (lzo_bytep) PyMem_Malloc(out_len > 0 ? out_len : 1);
    if (out == NULL)
    {
        Py_DECREF(result_str);
        return PyErr_NoMemory();
    }

    /* optimize */
#if PY_MAJOR_VERSION >= 3
    in = (lzo_bytep) PyBytes_AsString(result_str);
#else
    in = (lzo_bytep) PyString_AsString(result_str);
#endif

    Py_BEGIN_ALLOW_THREADS
    new_len = out_len;
    err = lzo1x_optimize( header ? in+5 : in, in_len, out, &new_len, NULL);
    Py_END_ALLOW_THREADS

    PyMem_Free(out);
    if (err != LZO_E_OK || (header && new_len != out_len))
    {
        Py_DECREF(result_str);
        PyErr_Format(LzoError, "Compressed data violation %i", err);
        return NULL;
    }

    /* success */
    return result_str;

header_error:
    PyErr_SetString(LzoError, "Header error - invalid compressed data");
    return NULL;
}


/***********************************************************************
// adler32
************************************************************************/

static /* const */ char adler32__doc__[] =
"adler32(string) -- Compute an Adler-32 checksum of string, using "
"a default starting value, and returning an integer value.\n"
"adler32(string, value) -- Compute an Adler-32 checksum of string, using "
"the starting value provided, and returning an integer value\n"
;

static PyObject *
adler32(PyObject *dummy, PyObject *args)
{
    char *buf;
    Py_ssize_t len;
    unsigned long val = 1; /* == lzo_adler32(0, NULL, 0); */

    UNUSED(dummy);
    if (!PyArg_ParseTuple(args, "s#|l", &buf, &len, &val))
        return NULL;
    if (len > 0)
    {
        Py_BEGIN_ALLOW_THREADS
        val = lzo_adler32((lzo_uint32)val, (const lzo_bytep)buf, len);
        Py_END_ALLOW_THREADS
    }

#if PY_MAJOR_VERSION >= 3
    return PyLong_FromLong(val);
#else
    return PyInt_FromLong(val);
#endif
}


/***********************************************************************
// crc32
************************************************************************/

static /* const */ char crc32__doc__[] =
"crc32(string) -- Compute a CRC-32 checksum of string, using "
"a default starting value, and returning an integer value.\n"
"crc32(string, value) -- Compute a CRC-32 checksum of string, using "
"the starting value provided, and returning an integer value.\n"
;

static PyObject *
crc32(PyObject *dummy, PyObject *args)
{
    char *buf;
    Py_ssize_t len;
    unsigned long val = 0; /* == lzo_crc32(0, NULL, 0); */

    UNUSED(dummy);
    if (!PyArg_ParseTuple(args, "s#|l", &buf, &len, &val))
        return NULL;
    if (len > 0)
        val = lzo_crc32((lzo_uint32)val, (const lzo_bytep)buf, len);
#if PY_MAJOR_VERSION >= 3
    return PyLong_FromLong(val);
#else
    return PyInt_FromLong(val);
#endif
}


/***********************************************************************
// main
************************************************************************/

static /* const */ PyMethodDef methods[] =
{
    {"adler32",    (PyCFunction)adler32,    METH_VARARGS, adler32__doc__},
    {"compress",   (PyCFunction)compress,   METH_VARARGS | METH_KEYWORDS, compress__doc__},
    {"crc32",      (PyCFunction)crc32,      METH_VARARGS, crc32__doc__},
    {"decompress", (PyCFunction)decompress, METH_VARARGS | METH_KEYWORDS, decompress__doc__},
    {"optimize",   (PyCFunction)optimize,   METH_VARARGS, optimize__doc__},
    {NULL, NULL, 0, NULL}
};


static /* const */ char module_documentation[]=
"The functions in this module allow compression and decompression "
"using the LZO library.\n\n"
"adler32(string)         -- Compute an Adler-32 checksum.\n"
"adler32(string, start)  -- Compute an Adler-32 checksum using a given starting value.\n"
"compress(string)        -- Compress a string.\n"
"compress(string, ...)   -- See help(lzo.compress) for more options.\n"
"crc32(string)           -- Compute a CRC-32 checksum.\n"
"crc32(string, start)    -- Compute a CRC-32 checksum using a given starting value.\n"
"decompress(string)      -- Decompresses a compressed string.\n"
"decompress(string, ...) -- See help(lzo.decompress) for more options.\n"
"optimize(string)        -- Optimize a compressed string.\n"
"optimize(string, ...)   -- See help(lzo.optimize) for more options.\n"
;

#if PY_MAJOR_VERSION >= 3
static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "lzo", /* name */
    "Python bindings for the LZO data compression library", /* doc */
    -1, /* size */
    methods, /* methods */
    NULL, /* reload */
    NULL, /* traverse */
    NULL, /* clear */
    NULL, /* free */
};
#endif

#ifdef _MSC_VER
_declspec(dllexport)
#endif
#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit_lzo(void)
#else
void initlzo(void)
#endif
{
    PyObject *m, *d, *v;

    if (lzo_init() != LZO_E_OK)
#if PY_MAJOR_VERSION >= 3
        return NULL;
#else
        return;
#endif

#if PY_MAJOR_VERSION >= 3
    m = PyModule_Create(&module);
#else
    m = Py_InitModule4("lzo", methods, module_documentation,
                       NULL, PYTHON_API_VERSION);
#endif
    d = PyModule_GetDict(m);

    LzoError = PyErr_NewException("lzo.error", NULL, NULL);
    PyDict_SetItemString(d, "error", LzoError);

#if PY_MAJOR_VERSION >= 3
    v = PyBytes_FromString("Markus F.X.J. Oberhumer <markus@oberhumer.com>");
#else
    v = PyString_FromString("Markus F.X.J. Oberhumer <markus@oberhumer.com>");
#endif

    PyDict_SetItemString(d, "__author__", v);
    Py_DECREF(v);
#if PY_MAJOR_VERSION >= 3
    v = PyBytes_FromString(MODULE_VERSION);
#else
    v = PyString_FromString(MODULE_VERSION);
#endif
    PyDict_SetItemString(d, "__version__", v);
    Py_DECREF(v);
#if PY_MAJOR_VERSION >= 3
    v = PyLong_FromLong((long)lzo_version());
#else
    v = PyInt_FromLong((long)lzo_version());
#endif
    PyDict_SetItemString(d, "LZO_VERSION", v);
    Py_DECREF(v);
#if PY_MAJOR_VERSION >= 3
    v = PyBytes_FromString(lzo_version_string());
#else
    v = PyString_FromString(lzo_version_string());
#endif
    PyDict_SetItemString(d, "LZO_VERSION_STRING", v);
    Py_DECREF(v);
#if PY_MAJOR_VERSION >= 3
    v = PyBytes_FromString(lzo_version_date());
#else
    v = PyString_FromString(lzo_version_date());
#endif
    PyDict_SetItemString(d, "LZO_VERSION_DATE", v);
    Py_DECREF(v);

#if PY_MAJOR_VERSION >= 3
    return m;
#endif
}


/*
vi:ts=4:et
*/
