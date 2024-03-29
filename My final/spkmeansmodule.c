#define PY_SSIZE_T_CLEAN  /* For all # variants of unit formats (s#, y#, etc.) use Py_ssize_t rather than int. */
#include <Python.h>       /* MUST include <Python.h>, this implies inclusion of the following standard headers:
                           <stdio.h>, <string.h>, <errno.h>, <limits.h>, <assert.h> and <stdlib.h> (if available). */
#include "spkmeans.h"

/*============================================================================
==============================================================================
== THIS WAS MY KMEANS IMPLEMENTATION, THE FUNCTIONS THEMSELVES NOT THE API! ==
============================ - END UP HERE - =================================
==============================================================================*/

static PyObject* GetMat(double **mat, int *size){
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
}

double **c_mat(PyObject *float_mat){
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

}


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
        return NULL;
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

static double *python_list_to_C_arr(PyObject *py_list){
    double *arr;
    int length_arr, i;
    /*PyObject *list;*/

    if(!PyList_Check(py_list)){
        return NULL;
    }/*end of if*/

    length_arr = PyList_Size(py_list);

    arr = (double *)calloc(length_arr, sizeof(double));

    if(!arr){/*calloc failed*/
        printf("An Error Has Occurred\n");
        return NULL;
    }

    for(i=0; i<length_arr; i++){
        arr[i] = PyFloat_AsDouble(PyList_GetItem(py_list,i));
            if(arr[i] == -1.0 && PyErr_Occurred()){
                printf("An Error Has Occurred\n");
                return NULL;
            }

    }/*end of for*/

    return arr;
}/*end of python_list_to_C_arr function*/

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

static PyObject* print_mat_normal_capi(PyObject *self, PyObject *args){
    PyObject *float_mat;
    double **mat;
    int row, col;

    if (!PyArg_ParseTuple(args, "Oii", &float_mat, &row, &col)){
        return NULL;
    }

    mat = python_mat_to_C_mat(float_mat);
    /*printf("row is: %d, col is: %d\n", row, col);*/
    print_mat_normal(mat, row, col);

    /*printf("\nfinish print\n");*/

    return Py_BuildValue("i", row);

}

/*void fix_mat_points(double **mat, int size){
    int i, j;


    for(i=0; i<size; i++){
        for(j=0; j<size; j++){
            mat[i][j] = floor(10000000*mat[i][j])/10000000;
        }
    }

}end of fix_mat_points function*/

static PyObject* file_to_mat_capi(PyObject *self, PyObject *args){

    char *file_name;
    int *mat_s;

    if (!PyArg_ParseTuple(args, "s", &file_name)){
        return NULL;
    }

    mat_s = mat_size(file_name);

    /*print_mat_normal(file_to_mat(file_name), mat_s[0], mat_s[1]);*/

    return C_mat_to_python_mat(file_to_mat(file_name), mat_s[0], mat_s[1]);

}/*end of file_to_mat_capi function*/


static PyObject* wam_capi(PyObject *self, PyObject *args){
    PyObject *float_mat;
    double **mat;
    int size[2];
    int row, col;

    if (!PyArg_ParseTuple(args, "Oii", &float_mat, &row, &col)){
        return NULL;
    }

    mat = python_mat_to_C_mat(float_mat);
    /*printf("one\n");
    print_mat_normal(mat, row, col);*/

    size[0]= row;
    size[1] = row;

    mat = wam_func(mat, row, col);

    /*printf("two\n");
    print_mat_normal(mat, row, row);

    
    printf("three\n");*/
    /*fix_mat_points(mat, row);*/

    /*printf("\n");*/

    return GetMat(mat, size);


}/*end of wam_capi function*/

static PyObject* wam_capionn(PyObject *self, PyObject *args){
    char *file_name;
    int *mat_s;
    double **c_mat;

    if (!PyArg_ParseTuple(args, "s", &file_name)){
        return NULL;
    }

    mat_s = mat_size(file_name);

    c_mat = file_to_mat(file_name);

    return GetMat(wam_func(c_mat, mat_s[0], mat_s[1]), mat_s);


}/*end of wam_capi function*/

static PyObject* ddg_capi(PyObject *self, PyObject *args)
{
    double **mat;
    int row, col;
    int size[2];
    PyObject *float_mat;

    if (!PyArg_ParseTuple(args, "Oii", &float_mat, &row, &col)){
        return NULL;
    }

    mat = python_mat_to_C_mat(float_mat);

    mat = ddg_func(mat, row, col);

    /*fix_mat_points(mat, row);*/

    size[0] = row;
    size[1] = row;

    return GetMat(mat, size);
}/*end of ddg_capi function*/

static PyObject* lnorm_capi(PyObject *self, PyObject *args)
{
    double **mat;
    int row, col;
    int size[2];
    PyObject *float_mat;

    if (!PyArg_ParseTuple(args, "Oii", &float_mat, &row, &col)){
        return NULL;
    }

    mat = python_mat_to_C_mat(float_mat);

    mat = lnorm_func(mat, row, col);

    /*fix_mat_points(mat, row);*/

    size[0] = row;
    size[1] = row;

    return GetMat(mat, size);
}/*end of lnorm_capi function*/

static PyObject* jacobi_capi(PyObject *self, PyObject *args)
{
    double **mat, **jac_mat;
    int size;/*Square matrix*/
    int size_arr[2];
    PyObject *float_mat;

    if (!PyArg_ParseTuple(args, "Oi", &float_mat, &size)){
        return NULL;
    }

    mat = python_mat_to_C_mat(float_mat);

    /*printf("\nmat before\n");
    print_mat_normal(mat, size, size);
    printf("\nmat before\n");*/

    jac_mat = jacobi_func(mat, size);

    /*fix_mat_points(mat, size);*/



    size_arr[0] = size+1;
    size_arr[1] = size;
    /*printf("\nmat\n");
    print_mat_normal(jac_mat, size+1, size);
    printf("\nmat\n");*/


    return GetMat(jac_mat, size_arr);
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
    double *index_mat;
    int row, col;
    PyObject *float_mat, *ind_matrix;

    if (!PyArg_ParseTuple(args, "iidOiiO", &k, &max_iter, &eps, &float_mat, &row, &col, &ind_matrix)){
        return NULL;
    }
    /*printf("we are in module\n");*/


    mat = python_mat_to_C_mat(float_mat);
    /*printf("we finish float mat\n"); */
    index_mat = python_list_to_C_arr(ind_matrix);

    /*printf("we are about to send to kmeans_double\n");
    printf("col is: %d\n", col);*/


/* This builds the answer ("d" = Convert a C double to a Python floating point number) back into a python object */
    return Py_BuildValue("i", kmeans_double(k, max_iter, eps, mat, row, col, index_mat)); /*  Py_BuildValue(...) returns a PyObject*  */
    
}/*end of kmeans_double_capi function*/

static PyObject* vectors_matrix_capi(PyObject *self, PyObject *args)
{
    double **mat;
    int size, k;/*Square matrix*/
    PyObject *float_mat;

    if (!PyArg_ParseTuple(args, "Oii", &float_mat, &size, &k)){
        return NULL;
    }

    /*printf("size is: %d, k is: %d\n", size, k);*/

    mat = python_mat_to_C_mat(float_mat);

    /*printf("\nmat\n");
    print_mat_normal(mat, size, size-1);
    printf("\nmat\n");*/

    return C_mat_to_python_mat(vectors_matrix(mat, size, k), size-1, k);
}/*end of vectors_matrix_capi function*/

static PyObject* vectors_matrix_another_try_capi(PyObject *self, PyObject *args)
{
    char *file_name;
    int k, row;/*Square matrix*/
    double **mat;

    if (!PyArg_ParseTuple(args, "sii", &file_name, &k, &row)){
        return NULL;
    }

    /*printf("size is: %d, k is: %d\n", size, k);*/

    /*printf("\nmat\n");
    print_mat_normal(mat, size, size-1);
    printf("\nmat\n");*/

    mat = alg_spk(file_name, k);

    return C_mat_to_python_mat(mat , row, k);
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
    {"print_mat_normal_capi",                   /* the Python method name that will be used */
      (PyCFunction) print_mat_normal_capi, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parametersaccepted for this function */
      PyDoc_STR("A geometric series up to n. sum_up_to_n(z^n)")}, /*  The docstring for the function *///NEED TO CHANGE DESCRIPTION

    {"file_to_mat_capi",                   /* the Python method name that will be used */
      (PyCFunction) file_to_mat_capi, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parametersaccepted for this function */
      PyDoc_STR("A geometric series up to n. sum_up_to_n(z^n)")}, /*  The docstring for the function *///NEED TO CHANGE DESCRIPTION

    {"wam_capionn",                   /* the Python method name that will be used */
      (PyCFunction) wam_capionn, /* the C-function that implements the Python function and returns static PyObject*  */
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

    {"vectors_matrix_another_try_capi",                   /* the Python method name that will be used */
      (PyCFunction) vectors_matrix_another_try_capi, /* the C-function that implements the Python function and returns static PyObject*  */
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