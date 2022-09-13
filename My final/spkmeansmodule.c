#define PY_SSIZE_T_CLEAN  /* For all # variants of unit formats (s#, y#, etc.) use Py_ssize_t rather than int. */
#include <Python.h>       /* MUST include <Python.h>, this implies inclusion of the following standard headers:
                           <stdio.h>, <string.h>, <errno.h>, <limits.h>, <assert.h> and <stdlib.h> (if available). */
#include "spkmeans.h"

/*============================================================================
==============================================================================
== THIS WAS MY KMEANS IMPLEMENTATION, THE FUNCTIONS THEMSELVES NOT THE API! ==
============================ - END UP HERE - =================================
==============================================================================*/

/*static PyObject* GetMat(double **mat, int *size){
    int m,n, i, j;
    PyObject* python_mat;
    m = size[0];
    n = size[1];
    python_mat = PyList_New(m);
    for (i = 0; i < m; ++i)
    {
        PyObject* python_row = PyList_New(n);
        for (j = 0; j < n; j++){
            double r = mat[i][j];
            PyObject* python_float = Py_BuildValue("d", r);
            PyList_SetItem(python_row, j, python_float);
        }
        PyList_SetItem(python_mat, i, python_row);
    }
    return python_mat;
}*/

/*double **c_mat(PyObject *float_mat){
    PyObject *row, *item;
    double **mat;
    int rows, columns,i ,j;


    rows = (int)PyObject_Length(float_mat);
    if (rows < 0){
        return NULL;
    }
    row = PyList_GetItem(float_mat, 0);
    columns = (int)PyObject_Length(row);
    
    mat = (double **) malloc(sizeof(double *) * rows);
    for (i = 0; i < rows; i++){
        mat[i] = (double *) malloc(sizeof(double) * columns);
    }
    for (i = 0; i < rows; i++){
        row = PyList_GetItem(float_mat, i);
        for (j = 0; j< columns; j++){
            item = PyList_GetItem(row, j);
            if (!PyFloat_Check(item))
                mat[i][j] = 0.0;
            mat[i][j] = PyFloat_AsDouble(item);
        }
    }

    return mat;

}*/


/*converting a python matrix to a C matrix*/
static double **python_mat_to_C_mat(PyObject *py_mat){

    double **mat;
    int i, j, n_rows, n_cols = 0;
    PyObject *list;

    if(!PyList_Check(py_mat)){
        return NULL;
    }/*end of if*/

    n_rows = PyList_Size(py_mat);

    if( n_rows){
        n_cols = PyList_Size(PyList_GetItem(py_mat, 0));
    }

    mat = (double **)calloc(n_rows, sizeof(double *));

    if(!mat){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    for(i=0; i<n_rows; i++){
        
        mat[i] = (double *)calloc(n_cols, sizeof(double));

        if(!mat[i]){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
        }
    }
    
    

    for(i=0; i<n_rows; i++){
        list = PyList_GetItem(py_mat, i);
        for(j=0; j<n_cols; j++){
            mat[i][j] = PyFloat_AsDouble(PyList_GetItem(list,j));
            if(mat[i][j] == -1.0 && PyErr_Occurred()){
                printf("An Error Has Occurred\n");
                return NULL;
            }

        }/*end of inner for*/

    }/*end of outer for*/

    return mat;

}/*end of python_mat_to_C_mat function*/

/*converting C matrix to a python matrix (list of lists)*/
static PyObject *C_mat_to_python_mat(double **mat, int row, int col){

    int i, j;
    PyObject *py_mat, *py_vector;

    py_mat = PyList_New(row);

    for(i=0; i<row; i++){

        py_vector = PyList_New(col);

        for(j=0; j<col; j++){
            PyList_SetItem(py_vector, j, Py_BuildValue("d", mat[i][j]));
        }/*end of inner for*/

        PyList_SetItem(py_mat, i, py_vector);

    }/*end of outer for*/

    return py_mat;

}/*end of C_mat_to_python_mat function*/


int *mat_sizei(PyObject *float_mat){
    int rows, columns, *size;
    PyObject *row;

    rows = (int)PyObject_Length(float_mat);
    row = PyList_GetItem(float_mat, 0);
    columns = (int)PyObject_Length(row);

    size = (int *) malloc(sizeof(int) * 2);
    size[0] = rows;
    size[1] = columns;

    return size;
}

/*
 * This actually defines the kmeans function using a wrapper C API function
 * The wrapping function needs a PyObject* self argument.
 * This is a requirement for all functions and methods in the C API.
 * It has input PyObject *args from Python.
 */
static PyObject* file_to_mat_capi(PyObject *self, PyObject *args){

    char *file_name;
    int *mat_s;

    if (!PyArg_ParseTuple(args, "s", &file_name)){
        return NULL;
    }

    mat_s = mat_size(file_name);

    return C_mat_to_python_mat(file_to_mat(file_name), mat_s[0], mat_s[1]);

}/*end of file_to_mat_capi function*/


static PyObject* wam_capi(PyObject *self, PyObject *args){
    PyObject *float_mat;
    double **mat;
    int row, col;

    if (!PyArg_ParseTuple(args, "Oii", &float_mat, &row, &col)){
        return NULL;
    }

    mat = python_mat_to_C_mat(float_mat);

    return C_mat_to_python_mat(wam_func(mat, row, col), row, col);


}/*end of wam_capi function*/

static PyObject* ddg_capi(PyObject *self, PyObject *args)
{
    double **mat;
    int row, col;
    PyObject *float_mat;

    if (!PyArg_ParseTuple(args, "Oii", &float_mat, &row, &col)){
        return NULL;
    }

    mat = python_mat_to_C_mat(float_mat);

    return C_mat_to_python_mat(ddg_func(mat, row, col), row, col);
}/*end of ddg_capi function*/

static PyObject* lnorm_capi(PyObject *self, PyObject *args)
{
    double **mat;
    int row, col;
    PyObject *float_mat;

    if (!PyArg_ParseTuple(args, "Oii", &float_mat, &row, &col)){
        return NULL;
    }

    mat = python_mat_to_C_mat(float_mat);

    return C_mat_to_python_mat(lnorm_func(mat, row, col), row, col);
}/*end of lnorm_capi function*/

static PyObject* jacobi_capi(PyObject *self, PyObject *args)
{
    double **mat;
    int size;/*Square matrix*/
    PyObject *float_mat;

    if (!PyArg_ParseTuple(args, "Oi", &float_mat, &size)){
        return NULL;
    }

    mat = python_mat_to_C_mat(float_mat);

    return C_mat_to_python_mat(jacobi_func(mat, size), size, size);
}/*end of jacobi_capi function*/

static PyObject* heuristic_capi(PyObject *self, PyObject *args){
    
    double **mat;
    int row, col;
    PyObject *float_mat;

    if (!PyArg_ParseTuple(args, "Oii", &float_mat, &row, &col)){
        return NULL;
    }

    mat = python_mat_to_C_mat(float_mat);


/* This builds the answer ("d" = Convert a C double to a Python floating point number) back into a python object */
    return Py_BuildValue("i", heuristic(mat, row, col)); /*  Py_BuildValue(...) returns a PyObject*  */

}/*end of function heuristic_capi*/

static PyObject* kmeans_double_capi(PyObject *self, PyObject *args)
{
    double eps;
    int k, max_iter = 300;
    double **mat;
    double **index_mat;
    int row, col;
    PyObject *float_mat, *ind_matrix;

    if (!PyArg_ParseTuple(args, "iidOiiO", &k, &max_iter, &eps, &float_mat, &row, &col, &ind_matrix)){
        return NULL;
    }



    mat = python_mat_to_C_mat(float_mat);
    index_mat = python_mat_to_C_mat(ind_matrix);


/* This builds the answer ("d" = Convert a C double to a Python floating point number) back into a python object */
    return Py_BuildValue("i", kmeans_double(k, max_iter, eps, mat, row, col, index_mat)); /*  Py_BuildValue(...) returns a PyObject*  */
    
}/*end of kmeans_double_capi function*/

static PyObject* vectors_matrix_capi(PyObject *self, PyObject *args)
{
    double **mat;
    int size, k;/*Square matrix*/
    PyObject *float_mat;

    if (!PyArg_ParseTuple(args, "Oi", &float_mat, &size, &k)){
        return NULL;
    }

    mat = python_mat_to_C_mat(float_mat);

    return C_mat_to_python_mat(vectors_matrix(mat, size, k), size, k);
}/*end of vectors_matrix_capi function*/


/*static PyObject* wam_capi(PyObject *self, PyObject *args)
{
    double **mat;
    int *size, *res_size;
    PyObject *float_mat;

    if (!PyArg_ParseTuple(args, "O", &float_mat)){
        return NULL;
    }

    mat = c_mat(float_mat);
    size = mat_sizei(float_mat);
    res_size = (int *) malloc(sizeof(int) * 2);
    res_size[0] = size[0];
    res_size[1] = size[0];

    return GetMat(wam_func(mat, size[0], size[1]), res_size);
}*/

/*static PyObject* ddg_capi(PyObject *self, PyObject *args)
{
    double **mat;
    int *size, *res_size;
    PyObject *float_mat;

    if (!PyArg_ParseTuple(args, "O", &float_mat)){
        return NULL;
    }

    mat = c_mat(float_mat);
    size = mat_sizei(float_mat);
    res_size = (int *) malloc(sizeof(int) * 2);
    res_size[0] = size[0];
    res_size[1] = size[0];

    return GetMat(ddg_func(mat, size[0], size[1]), res_size);
}*/

/*static PyObject* lnorm_capi(PyObject *self, PyObject *args)
{
    double **mat;
    int *size, *res_size;
    PyObject *float_mat;

    if (!PyArg_ParseTuple(args, "O", &float_mat)){
        return NULL;
    }

    mat = c_mat(float_mat);
    size = mat_sizei(float_mat);
    res_size = (int *) malloc(sizeof(int) * 2);
    res_size[0] = size[0];
    res_size[1] = size[0];

 This builds the answer ("d" = Convert a C double to a Python floating point number) back into a python object */
    /*return Py_BuildValue("d", test_ctopy(d));*/ /*  Py_BuildValue(...) returns a PyObject*  */

   /* return GetMat(lnorm_func(mat, size[0], size[1]), res_size);
}*/





/*
 * This array tells Python what methods this module has.
 * We will use it in the next structure
 */
static PyMethodDef capiMethods[] = {
    {"file_to_mat_capi",                   /* the Python method name that will be used */
      (PyCFunction) file_to_mat_capi, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parametersaccepted for this function */
      PyDoc_STR("A geometric series up to n. sum_up_to_n(z^n)")}, /*  The docstring for the function *///NEED TO CHANGE DESCRIPTION

    {"wam_capi",                   /* the Python method name that will be used */
      (PyCFunction) wam_capi, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parametersaccepted for this function */
      PyDoc_STR("A geometric series up to n. sum_up_to_n(z^n)")}, /*  The docstring for the function *///NEED TO CHANGE DESCRIPTION

    {"ddg_capi",                   /* the Python method name that will be used */
      (PyCFunction) ddg_capi, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parametersaccepted for this function */
      PyDoc_STR("A geometric series up to n. sum_up_to_n(z^n)")}, /*  The docstring for the function *///NEED TO CHANGE DESCRIPTION

    {"lnorm_capi",                   /* the Python method name that will be used */
      (PyCFunction) lnorm_capi, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parametersaccepted for this function */
      PyDoc_STR("A geometric series up to n. sum_up_to_n(z^n)")}, /*  The docstring for the function *///NEED TO CHANGE DESCRIPTION

    {"jacobi_capi",                   /* the Python method name that will be used */
      (PyCFunction) jacobi_capi, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parametersaccepted for this function */
      PyDoc_STR("A geometric series up to n. sum_up_to_n(z^n)")}, /*  The docstring for the function *///NEED TO CHANGE DESCRIPTION

    {"heuristic_capi",                   /* the Python method name that will be used */
      (PyCFunction) heuristic_capi, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parametersaccepted for this function */
      PyDoc_STR("A geometric series up to n. sum_up_to_n(z^n)")}, /*  The docstring for the function *///NEED TO CHANGE DESCRIPTION

    {"kmeans_double_capi",                   /* the Python method name that will be used */
      (PyCFunction) kmeans_double_capi, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parametersaccepted for this function */
      PyDoc_STR("A geometric series up to n. sum_up_to_n(z^n)")}, /*  The docstring for the function *///NEED TO CHANGE DESCRIPTION

    {"vectors_matrix_capi",                   /* the Python method name that will be used */
      (PyCFunction) vectors_matrix_capi, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parametersaccepted for this function */
      PyDoc_STR("A geometric series up to n. sum_up_to_n(z^n)")}, /*  The docstring for the function *///NEED TO CHANGE DESCRIPTION

    {NULL, NULL, 0, NULL}     /* The last entry must be all NULL as shown to act as a
                                 sentinel. Python looks for this entry to know that all
                                 of the functions for the module have been defined. */
};


/* This initiates the module using the above definitions. */
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp", /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,  /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    capiMethods /* the PyMethodDef array from before containing the methods of the extension */
};


/*
 * The PyModuleDef structure, in turn, must be passed to the interpreter in the module’s initialization function.
 * The initialization function must be named PyInit_name(), where name is the name of the module and should match
 * what we wrote in struct PyModuleDef.
 * This should be the only non-static item defined in the module file
 */
PyMODINIT_FUNC
PyInit_mykmeanssp(void)
{
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m) {
        return NULL;
    }
    return m;
}