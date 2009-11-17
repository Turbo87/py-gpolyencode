#include <Python.h>
#include "structmember.h"

#include "GPolyEncoder.h"

using namespace std;

typedef struct {
    PyObject_HEAD
    GPolyEncoder* gpe;
} GPolyEncoderPy;

static int
GPolyEncoderPy_init(GPolyEncoderPy *self, PyObject *args, PyObject *kwargs)
{
    // FIXME: how to specify method signature?
    // defaults
    int numLevels=18;
    int zoomFactor=2;
    double threshold=0.00001;
    int forceEndpoints=1;
    
    static char *kwlist[] = {"num_levels", "zoom_factor", "threshold", "force_endpoints", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|iidi", kwlist, 
                                      &numLevels, &zoomFactor, &threshold, &forceEndpoints))
        return NULL; 

    self->gpe = new GPolyEncoder(numLevels, zoomFactor, threshold, forceEndpoints);
    return 0;
}

static void
GPolyEncoderPy_dealloc(GPolyEncoderPy *self) {
    delete self->gpe;
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject*
GPolyEncoderPy_encode(GPolyEncoderPy *self, PyObject *args) {
    // FIXME: how to specify method signature?
    PyObject *p_points;
    if (!PyArg_ParseTuple(args, "O", &p_points))
        return NULL;
    
    // Check we actually have a sequence of two-tuples and populate a
    // std::vector with the values. It'd be nice to pass the values through
    // without iterating needlessly...
    if (!(p_points = PySequence_Fast(p_points, "expected sequence type"))) {
        return NULL;
    }
    Py_ssize_t nPoints = PySequence_Fast_GET_SIZE(p_points);
    vector<pair<double,double> > n_points;
    for (Py_ssize_t i=0; i<nPoints; i++) {
        // check the item is a tuple
        PyObject *p_vertices = PySequence_Fast(PySequence_Fast_GET_ITEM(p_points, i), "expected sequence type");
        if (!p_vertices) {
            return NULL;
        }
        // and its length is at least 2 (ignore 3D coordinates if present)
        if (PySequence_Fast_GET_SIZE(p_vertices) < 2) {
            PyErr_SetString(PyExc_TypeError, "expected two-tuple with numbers ((x0, y0), (x1, y1), ...)");
            return NULL;
        } 
        // get the first two values from the item and check they're numbers
        double n_c[2];
        for (int j=0; j<2; j++) {
            PyObject *p_c = PySequence_Fast_GET_ITEM(p_vertices, j);
            if (!PyNumber_Check(p_c)) {
                PyErr_SetString(PyExc_TypeError, "expected two-tuple with numbers ((x0, y0), (x1, y1), ...)");
                return NULL;
            }
            // get as double
            PyObject *pf_c = PyNumber_Float(p_c);
            n_c[j] = PyFloat_AsDouble(pf_c);
            Py_DECREF(pf_c);
        }
        // add to our vector
        n_points.push_back(pair<double,double>(n_c[0], n_c[1]));
        Py_DECREF(p_vertices);
    }
    Py_DECREF(p_points);
    
    // do our encoding
    auto_ptr<pair<string, string> > n_result;
    // might as well allow some other threads to run...
    Py_BEGIN_ALLOW_THREADS
    n_result = self->gpe->dpEncode(n_points);
    Py_END_ALLOW_THREADS
    
    // build a dictionary of the results
    // {'points':'...', 'levels':'...', 'numLevels':#, 'zoomFactor':#}
    PyObject *p_result = PyDict_New();
    PyObject *ep, *el, *zf, *nl;
    
    ep = PyString_FromString(n_result->first.c_str());
    if (PyDict_SetItemString(p_result, "points", ep))
        return NULL;
    Py_DECREF(ep);
    
    el = PyString_FromString(n_result->second.c_str());
    if (PyDict_SetItemString(p_result, "levels", el))
        return NULL;
    Py_DECREF(el);
    
    zf = PyInt_FromLong(self->gpe->getZoomFactor());
    if (PyDict_SetItemString(p_result, "zoomFactor", zf))
        return NULL;
    Py_DECREF(zf);
    
    nl = PyInt_FromLong(self->gpe->getNumLevels());
    if (PyDict_SetItemString(p_result, "numLevels", nl))
        return NULL;
    Py_DECREF(nl);
    
    return p_result;
}


static PyMemberDef GPolyEncoderPy_members[] = {
    {NULL}  /* Sentinel */
};

static PyMethodDef GPolyEncoderPy_methods[] = {
    {"encode", (PyCFunction)GPolyEncoderPy_encode, METH_VARARGS, "Encode a sequence of points"},
    {NULL}  /* Sentinel */
};

static PyTypeObject GPolyEncoderPyType = {
    PyObject_HEAD_INIT(NULL)
    0,                                    /*ob_size*/
    "cgpolyencode.GPolyEncoder",          /*tp_name*/
    sizeof(GPolyEncoderPy),               /*tp_basicsize*/
    0,                                    /*tp_itemsize*/
    (destructor)GPolyEncoderPy_dealloc,   /*tp_dealloc*/
    0,                                    /*tp_print*/
    0,                                    /*tp_getattr*/
    0,                                    /*tp_setattr*/
    0,                                    /*tp_compare*/
    0,                                    /*tp_repr*/
    0,                                    /*tp_as_number*/
    0,                                    /*tp_as_sequence*/
    0,                                    /*tp_as_mapping*/
    0,                                    /*tp_hash */
    0,                                    /*tp_call*/
    0,                                    /*tp_str*/
    0,                                    /*tp_getattro*/
    0,                                    /*tp_setattro*/
    0,                                    /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,       /*tp_flags*/
    "Google Maps Polyline Encoder",       /* tp_doc */
    0,                                    /* tp_traverse */
    0,                                    /* tp_clear */
    0,                                    /* tp_richcompare */
    0,                                    /* tp_weaklistoffset */
    0,                                    /* tp_iter */
    0,                                    /* tp_iternext */
    GPolyEncoderPy_methods,               /* tp_methods */
    GPolyEncoderPy_members,               /* tp_members */
    0,                                    /* tp_getset */
    0,                                    /* tp_base */
    0,                                    /* tp_dict */
    0,                                    /* tp_descr_get */
    0,                                    /* tp_descr_set */
    0,                                    /* tp_dictoffset */
    (initproc)GPolyEncoderPy_init,        /* tp_init */
    0,                                    /* tp_alloc */
    PyType_GenericNew,                    /* tp_new */
};

static PyMethodDef cgpolyencode_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC  /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initcgpolyencode(void) 
{
    PyObject* m;

    if (PyType_Ready(&GPolyEncoderPyType) < 0)
        return;

    m = Py_InitModule3("cgpolyencode", cgpolyencode_methods,
                       "Google Maps Polyline encoding (C extension)");
                       
    if (m == NULL)
        return;

    Py_INCREF(&GPolyEncoderPyType);
    PyModule_AddObject(m, "GPolyEncoder", (PyObject *)&GPolyEncoderPyType);
}

