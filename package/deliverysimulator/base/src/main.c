#include <Python.h>
#include "engine/engine.h"
#include "util/errcode.h"

#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))

struct module_state {
    PyObject* error;
};

static PyObject* run(PyObject* self, PyObject* args) {
    double time_end;
    long seed;
    long num_vehicles;
    double vehicle_cost;
    PyObject *unit_cost_raw;
    double unit_cost[3];
    PyObject *unit_revenue_raw;
    double unit_revenue[3];
    long size;
    PyObject *tolerance_raw;
    double *tolerance;
    PyObject *from_raw;
    uint64_t *from;
    PyObject *to_raw;
    uint64_t *to;
    PyObject *distance_raw;
    double *distance;
    long edge;
    PyObject *rate_raw;
    double *rate;
    PyObject *upper_limit_raw;
    uint32_t *upper_limit;
    PyObject *production_rate_raw;
    double production_rate[3];
    long iter;
    long verbosity;
    PyObject *profit_raw;
    double *profit;
    PyObject *num_orders_raw;
    uint64_t *num_orders;
    PyObject *num_delivered_raw;
    uint64_t *num_delivered;
    if (!PyArg_ParseTuple(args,
                          "fllfO!O!lO!O!O!O!lO!O!O!llO!O!O!",
                          &time_end,
                          &seed,
                          &num_vehicles,
                          &vehicle_cost,
                          &PyList_Type, &unit_cost_raw,
                          &PyList_Type, &unit_revenue_raw,
                          &size,
                          &PyList_Type, &tolerance_raw,
                          &PyList_Type, &from_raw,
                          &PyList_Type, &to_raw,
                          &PyList_Type, &distance_raw,
                          &edge,
                          &PyList_Type, &rate_raw,
                          &PyList_Type, &upper_limit_raw,
                          &PyList_Type, &production_rate_raw,
                          &iter,
                          &verbosity,
                          &PyList_Type, &profit_raw,
                          &PyList_Type, &num_orders_raw,
                          &PyList_Type, &num_delivered_raw)) {
        return NULL;
    }
    double *iter_unit_cost = unit_cost;
    double *iter_unit_revenue = unit_revenue;
    double *iter_production_rate = production_rate;
    for (Py_ssize_t i = 0;
         i < 3;
         ++i,
         ++iter_unit_cost,
         ++iter_unit_revenue,
         ++iter_production_rate) {
        *iter_unit_cost =
            PyFloat_AsDouble(PyList_GetItem(unit_cost_raw, i));
        *iter_unit_revenue =
            PyFloat_AsDouble(PyList_GetItem(unit_revenue_raw, i));
        *iter_production_rate =
            PyFloat_AsDouble(PyList_GetItem(production_rate_raw, i));
    }
    tolerance = malloc(sizeof(double) * size);
    if (!tolerance) {
        PyErr_SetString(PyExc_RuntimeError, "Out of Memory");
        return NULL;
    }
    rate = malloc(sizeof(double) * size);
    if (!rate) {
        PyErr_SetString(PyExc_RuntimeError, "Out of Memory");
        free(tolerance);
        return NULL;
    }
    upper_limit = malloc(sizeof(uint32_t) * size * 3);
    if (!upper_limit) {
        PyErr_SetString(PyExc_RuntimeError, "Out of Memory");
        free(tolerance);
        free(rate);
        return NULL;
    }
    double *iter_tolerance = tolerance;
    double *iter_rate = rate;
    uint32_t *iter_upper_limit = upper_limit;
    for (Py_ssize_t i = 0;
         i < size;
         ++i,
         ++iter_tolerance,
         ++iter_rate) {
        *iter_tolerance =
            PyFloat_AsDouble(PyList_GetItem(tolerance_raw, i));
        *iter_rate =
            PyFloat_AsDouble(PyList_GetItem(rate_raw, i));
        PyObject *cur = PyList_GetItem(upper_limit_raw, i);
        for (Py_ssize_t j = 0;
             j < 3;
             ++j,
             ++iter_upper_limit) {
            *iter_upper_limit =
                PyLong_AsLong(PyList_GetItem(cur, j));
        }
    }
    from = malloc(sizeof(uint64_t) * edge);
    if (!from) {
        PyErr_SetString(PyExc_RuntimeError, "Out of Memory");
        free(tolerance);
        free(rate);
        free(upper_limit);
        return NULL;
    }
    to = malloc(sizeof(uint64_t) * edge);
    if (!to) {
        PyErr_SetString(PyExc_RuntimeError, "Out of Memory");
        free(tolerance);
        free(rate);
        free(upper_limit);
        free(from);
        return NULL;
    }
    distance = malloc(sizeof(double) * edge);
    if (!distance) {
        PyErr_SetString(PyExc_RuntimeError, "Out of Memory");
        free(tolerance);
        free(rate);
        free(upper_limit);
        free(from);
        free(to);
        return NULL;
    }
    uint64_t *iter_from = from;
    uint64_t *iter_to = to;
    double *iter_distance = distance;
    for (Py_ssize_t i = 0;
         i < edge;
         ++i,
         ++iter_from,
         ++iter_to,
         ++iter_distance) {
        *iter_from = PyLong_AsLong(PyList_GetItem(from_raw, i));
        *iter_to = PyLong_AsLong(PyList_GetItem(to_raw, i));
        *iter_distance = PyFloat_AsDouble(PyList_GetItem(distance_raw, i));
    }
    struct Engine engine;
    int ret = engine_initialize(&engine,
                                3600,
                                100000,
                                3,
                                10000,
                                unit_cost,
                                unit_revenue,
                                5,
                                tolerance,
                                from,
                                to,
                                distance,
                                7,
                                rate,
                                upper_limit,
                                production_rate);
    if (ret) {
        PyErr_SetString(PyExc_RuntimeError, errcode_lookup[ret]);
        engine_finalize(&engine);
        free(tolerance);
        free(rate);
        free(upper_limit);
        free(from);
        free(to);
        free(distance);
        return NULL;
    }
    profit = malloc(sizeof(double) * iter);
    if (!profit) {
        PyErr_SetString(PyExc_RuntimeError, "Out of Memory");
        free(tolerance);
        free(rate);
        free(upper_limit);
        free(from);
        free(to);
        free(distance);
        return NULL;
    }
    num_orders = malloc(sizeof(uint64_t) * iter);
    if (!num_orders) {
        PyErr_SetString(PyExc_RuntimeError, "Out of Memory");
        free(tolerance);
        free(rate);
        free(upper_limit);
        free(from);
        free(to);
        free(distance);
        free(profit);
        return NULL;
    }
    num_delivered = malloc(sizeof(uint64_t) * iter);
    if (!num_delivered) {
        PyErr_SetString(PyExc_RuntimeError, "Out of Memory");
        free(tolerance);
        free(rate);
        free(upper_limit);
        free(from);
        free(to);
        free(distance);
        free(profit);
        free(num_orders);
        return NULL;
    }
    ret = engine_run(&engine,
                     iter,
                     verbosity,
                     profit,
                     num_orders,
                     num_delivered);
    if (ret) {
        PyErr_SetString(PyExc_RuntimeError, errcode_lookup[ret]);
        engine_finalize(&engine);
        free(tolerance);
        free(rate);
        free(upper_limit);
        free(from);
        free(to);
        free(distance);
        free(profit);
        free(num_orders);
        free(num_delivered);
        return NULL;
    }
    engine_finalize(&engine);
    free(tolerance);
    free(rate);
    free(upper_limit);
    free(from);
    free(to);
    free(distance);
    double *iter_profit = profit;
    uint64_t *iter_num_orders = num_orders;
    uint64_t *iter_num_delivered = num_delivered;
    for (Py_ssize_t i = 0;
         i < iter;
         ++i,
         ++iter_profit,
         ++iter_num_orders,
         ++iter_num_delivered) {
        PyList_SetItem(profit_raw, i,
                       PyFloat_FromDouble(*iter_profit));
        PyList_SetItem(num_orders_raw, i,
                       PyLong_FromLong(*iter_num_orders));
        PyList_SetItem(num_delivered_raw, i,
                       PyLong_FromLong(*iter_num_delivered));
    }
    free(profit);
    free(num_orders);
    free(num_delivered);
    Py_RETURN_NONE;
}


static PyMethodDef _base_methods[] = {
    {"run", (PyCFunction)run, METH_VARARGS, NULL},
    {0}
};

static int _base_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int _base_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "deliverysimulator_base",
    NULL,
    sizeof(struct module_state),
    _base_methods,
    NULL,
    _base_traverse,
    _base_clear,
    NULL
};

PyMODINIT_FUNC PyInit__base(void) {
    PyObject *module = PyModule_Create(&moduledef);
    if (!module) return NULL;
    return module;
}
