/*
 * _parsesqlmodule.c
 *
 * paul cannon 2009 <paul@mozy.com>
 */

#include <Python.h>
#include <structmember.h>
#include "parser/adapt.h"
#include "parser/per_scanner_data.h"
#include "parser/gramparse.h"
#include "normalize.h"

typedef struct {
    PyObject_HEAD
    yyscan_t yyscanner;
    YYSTYPE* lval;
} scannerobject;

static PyObject*
scanner_getnext(scannerobject* self, PyObject* args)
{
    const char* tokname = NULL;
    char op[2] = "\0\0";
    PyObject* token = NULL;
    PyObject* value = NULL;
    PyObject* result = NULL;
    int toknum;

    switch ((toknum = lexer_next(self->yyscanner)))
    {
    case 0:
        Py_RETURN_NONE;
    case ICONST:
        tokname = "ICONST";
        value = PyInt_FromLong(self->lval->ival);
        break;
    case PARAM:
        tokname = "PARAM";
        value = PyInt_FromLong(self->lval->ival);
        break;
    case TYPECAST:
        tokname = "TYPECAST";
        value = PyString_FromString("::");
        break;
    case BCONST:
        tokname = "BCONST";
        break;
    case XCONST:
        tokname = "XCONST";
        break;
    case FCONST:
        tokname = "FCONST";
        break;
    case SCONST:
        tokname = "SCONST";
        break;
    case KEYWORD:
        tokname = "KEYWORD";
        break;
    case IDENT:
        tokname = "IDENT";
        break;
    case Op:
        tokname = "OP";
        break;
    default:
        tokname = "OP";
        op[0] = toknum;
        value = PyString_FromString(op);
    }
    if (value == NULL)
    {
        value = PyString_FromString(self->lval->str);
        free(self->lval->str);
    }
    token = PyString_FromString(tokname);
    result = PyTuple_Pack(2, token, value);
    Py_DECREF(value);
    Py_DECREF(token);
    return result;
}

static void
scanner_dealloc(scannerobject* self)
{
    lexer_destroy(self->yyscanner);
    self->ob_type->tp_free((PyObject*) self);
}

static PyObject*
scanner_new(PyTypeObject* subtype, PyObject* args, PyObject* kwargs)
{
    scannerobject* scanner;
    const char* sql = NULL;
    static char* kwlist[] = {"sql", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s", kwlist, &sql))
        return NULL;
    scanner = (scannerobject*) subtype->tp_alloc(subtype, 0);
    if (scanner == NULL)
        return NULL;

    if ((scanner->yyscanner = lexer_init(sql)) == NULL)
    {
        subtype->tp_free((PyObject*) scanner);
        PyErr_SetString(PyExc_OSError, "could not init scanner");
        return NULL;
    }
    scanner->lval = lexer_lval(scanner->yyscanner);
    return (PyObject*) scanner;
}

static PyMethodDef ScannerMethods[] = {
    {"getnext", (PyCFunction) scanner_getnext, METH_VARARGS,
     "Retrieve the next identified token from the scanner, as a tuple of "
     "the token name and the associated string from the source sql. If "
     "there are no more tokens, returns None."},
    {NULL, NULL, 0, NULL},
};

static PyTypeObject scanner_type = {
    PyObject_HEAD_INIT(NULL)
    0,                                      /*ob_size*/
    "parsesql.sqlscanner",                  /*tp_name*/
    sizeof(scannerobject),                  /*tp_basicsize*/
    0,                                      /*tp_itemsize*/
    (destructor)scanner_dealloc,            /*tp_dealloc*/
    0,                                      /*tp_print*/
    0,                                      /*tp_getattr*/
    0,                                      /*tp_setattr*/
    0,                                      /*tp_compare*/
    0,                                      /*tp_repr*/
    0,                                      /*tp_as_number*/
    0,                                      /*tp_as_sequence*/
    0,                                      /*tp_as_mapping*/
    0,                                      /*tp_hash */
    0,                                      /*tp_call*/
    0,                                      /*tp_str*/
    0,                                      /*tp_getattro*/
    0,                                      /*tp_setattro*/
    0,                                      /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "SQL scanner object",                   /*tp_doc*/
    0,		                                /*tp_traverse*/
    0,		                                /*tp_clear*/
    0,		                                /*tp_richcompare*/
    0,		                                /*tp_weaklistoffset*/
    0,		                                /*tp_iter*/
    0,		                                /*tp_iternext*/
    ScannerMethods,                         /*tp_methods*/
    0,                                      /*tp_members*/
    0,                                      /*tp_getset*/
    0,                                      /*tp_base*/
    0,                                      /*tp_dict*/
    0,                                      /*tp_descr_get*/
    0,                                      /*tp_descr_set*/
    0,                                      /*tp_dictoffset*/
    0,                                      /*tp_init*/
    0,                                      /*tp_alloc*/
    (newfunc)scanner_new,                   /*tp_new*/
};

static PyObject*
normalize_q_py(PyObject* self, PyObject* args, PyObject* kwargs)
{
    char* buf;
    const char* sql;
    int res;
    int alloclen;
    PyObject* p_remove_const = NULL;
    PyObject* result = NULL;
    int remove_const = 0;

    static char* kwlist[] = {"sql", "remove_const", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|O", kwlist,
                                     &sql, &p_remove_const))
        return NULL;

    if (p_remove_const != NULL && PyObject_IsTrue(p_remove_const))
        remove_const = 1;

    alloclen = strlen(sql) * 2;
    while (1)
    {
        buf = (char*) malloc(alloclen);
        res = normalize_q(sql, buf, alloclen, remove_const);
        if (res < 0)
        {
            free(buf);
            return PyErr_SetFromErrno(PyExc_RuntimeError);
        }
        if (res == 0)
            break;
        free(buf);
        alloclen *= 2;
    }

    result = PyString_FromString(buf);
    free(buf);
    return result;
}

static PyMethodDef module_methods[] = {
    {"normalize", (PyCFunction) normalize_q_py, METH_VARARGS | METH_KEYWORDS,
     "Given an SQL string, return a version of that query with all whitespace"
     " and character escapes normalized. Optionally also removes literal"
     " constants and replaces them with '?'."},
    {NULL,} /* Sentinel */
};

PyMODINIT_FUNC
init_parsesql(void)
{
    PyObject* m;

    if (PyType_Ready(&scanner_type) < 0)
        return;

    if ((m = Py_InitModule("_parsesql", module_methods)) == NULL)
        return;

    Py_INCREF(&scanner_type);
    PyModule_AddObject(m, "sqlscanner", (PyObject*)&scanner_type);
}

/* vim: set et sw=4 ts=4 : */
