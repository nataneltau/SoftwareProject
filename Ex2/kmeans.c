#define PY_SSIZE_T_CLEAN  /* For all # variants of unit formats (s#, y#, etc.) use Py_ssize_t rather than int. */
#include <Python.h>       /* MUST include <Python.h>, this implies inclusion of the following standard headers:
                             <stdio.h>, <string.h>, <errno.h>, <limits.h>, <assert.h> and <stdlib.h> (if available). */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



/*============================================================================
==============================================================================
== THIS IS MY KMEANS IMPLEMENTATION, THE FUNCTIONS THEMSELVES NOT THE API! ===
============================ - START DOWN HERE - =============================
==============================================================================*/


const int DEFAULT_ITER = 200;


/*maybe add a func that will count the numbers of lines(point) in the file
that way i will able to allocate arrays and not work with lists, can use it in
while(line 32) and in array line 51*/

/*this function make an array of string to an array of double, it basically makes the vektor from string representation to double one and puts it in vektor_double*/
void makeDataDuoble(int line_len, const char *vektor_char, int dimension, double *vektor_double){

    char *copy_vektor_char;
    double num;
    int k;
    char *token;

    copy_vektor_char = (char *)calloc(line_len, sizeof(char));
    /*memset(copy_vektor_char, 0, sizeof(copy_vektor_char));*/

    if(!copy_vektor_char){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    strncpy(copy_vektor_char, vektor_char, line_len);

    
    token = strtok(copy_vektor_char, ",");
    k = 0;
    
    /*if(token == NULL){
       printf("Nulli\n");
    }*/

    /*printf("token: %s\n", token);*/

    while(token != NULL && k<dimension){/*convert from array of strings to array of doubles*/
        num = atof(token);
        vektor_double[k] = num;
        k++;
        /*printf("token is: %s\n", token);*/
        token = strtok(NULL, ",");

    }/*end of while*/

    free(copy_vektor_char);

}/*end of function makeDataDuoble*/

int ftoa(double n, char* res, int afterpoint, int index);/*convert double to string*/

double calc_dist(const char *x, char *centroid, int dimension, int line_len);/*this function calculate (x-centroid)^2*/

int calc_argmin(int k, int line_len, const char *x, char **arr_centroids, int dimension);/*return index j for S_j: argmin*/

int calcLineLen(char *input_file);/*calculate the maximum line len*/

int numbersOfLines(char *input_file, int line_len);/*return the numbers of lines in the file*/

/*let's build some functions*/
int euclidean_norm( int k, int line_len, char **arr_centroids, char **arr_prev_centroids, int dimension, int iter_num){/*check condition to while loop in kmeans function*/

    double small_than_e = 0.001;
    double tmp;
    int i;

    if(iter_num == 0){/*we are at the first iteration, return false*/
        return 0;
    }


    for(i = 0; i<k; i++){/*each iteration check if there is a centroid which his euclidean norm bigger than 0.001*/

        tmp = calc_dist(arr_prev_centroids[i], arr_centroids[i], dimension, line_len);
        tmp = fabs(tmp);
        tmp = sqrt(tmp);

        if(tmp >= small_than_e){/*if this true than there is at least one centroid which his euclidean norm bigger than 0.001*/
            return 0;
        }/*end of if*/


    }/*end of for*/
    
    return 1;
}/*end of function euclidean_norm*/

/*this func calculate the new centroids, like line6 in the alg in ex1*/
void update_centroids(double clus_len, int line_len, int data_num, char **cluster, int dimension, char *str_centroid_buffer){

    double *the_new_centroid;
    char *tmp_buffer;
    double *clus_double;
    int m, i, j, n, k, l;
    int index, last_time;

    the_new_centroid = (double *)calloc(dimension, sizeof(double));

    if(!the_new_centroid){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    clus_double = (double *)calloc(dimension, sizeof(double));

   if(!clus_double){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    if(clus_len == 0){/*cluster to this centroid is empty, never should get in this block*/
        printf("An Error Has Occurred\n");
        exit(1);
    }/*end of if*/

    if(data_num > 0){/*for compiler*/
        l = 1;
    }

    for(l =0; l<dimension; l++){/*initialization*/
        the_new_centroid[l]=0;
    }/*end of for*/


    for(i=0; i<clus_len; i++){/*sum each coordinate for all the vektors in the cluster*/

        free(clus_double);
        clus_double = (double *)calloc(dimension, sizeof(double));

        if(!clus_double){/*calloc failed*/
            printf("An Error Has Occurred\n");
            exit(1);
        }

        makeDataDuoble(line_len, cluster[i], dimension, clus_double);


        for(j=0; j<dimension; j++){

            the_new_centroid[j] += clus_double[j];

        }/*end of inner for*/

    }/*end of for*/


    for(n=0; n<dimension; n++){/*normalization*/
        the_new_centroid[n] = the_new_centroid[n] / clus_len;
    }/*end of for*/

    index = 0;
    last_time = 0;


    tmp_buffer = (char *)calloc(line_len, sizeof(char));

    if(!tmp_buffer){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    /*memset(tmp_buffer, 0, sizeof(tmp_buffer));*/


    for(k = 0; k<dimension-1; k++){/*convert the new centroid to a string*/
        /*index = ftoa(the_new_centroid[k], str_centroid_buffer, 4, index);
        /str_centroid_buffer[index] = ',';
        /index++;*/

        index = sprintf(tmp_buffer, "%.4f,", the_new_centroid[k]);

        /*printf("the tmp_buffer: %s\n", tmp_buffer);*/

        for(m = 0; m<index; m++){
            str_centroid_buffer[m+last_time] = tmp_buffer[m];
        }
        /*printf("the str_centroid_buffer: %s\n", str_centroid_buffer);*/
        last_time += index;

    }/*end of for*/

    /*index = ftoa(the_new_centroid[dimension-1], str_centroid_buffer, 4, index);*/
    index = sprintf(tmp_buffer, "%.4f", the_new_centroid[dimension-1]);

        /*printf("the tmp_buffer: %s\n", tmp_buffer);*/

        for(m = 0; m<index; m++){
            str_centroid_buffer[m+last_time] = tmp_buffer[m];
        }
        /*printf("the str_centroid_buffer: %s\n", str_centroid_buffer);*/
        last_time += index;
    str_centroid_buffer[last_time] = '\0';
    /*printf("the final!!!: %s\n", str_centroid_buffer);*/

    free(clus_double);
    free(the_new_centroid);
    free(tmp_buffer);

}/*end of function update_centroids*/

int kmeans(int k, int max_iter, char *input_file, char *output_file){

    int iter_num = 0;
    int line_len = calcLineLen(input_file);
    int data_num = numbersOfLines(input_file, line_len);
    char *buff;
    char **data_points;
    int dimension=0;
    char **arr_centroids;
    char **arr_prev_centroids;
    char ***arr_clusters;
    int *index_for_cluster_k;
    int f, j, i, m, l;
    int index_to_cluster;
    char *str_centroid_buffer;
    FILE *fr, *fw;


    if(line_len <= 0 || data_num <= 0 || k <= 0 || max_iter <= 0){
        printf("Invalid Input!\n");
        exit(1);
    }

    str_centroid_buffer = (char *)calloc(line_len, sizeof(char));

    if(!str_centroid_buffer){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    buff = (char *)calloc(line_len, sizeof(char));

    if(!buff){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    data_points = (char **)calloc(data_num, sizeof(char*));

    if(!data_points){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    arr_centroids = (char **)calloc(k, sizeof(char*));

    if(!arr_centroids){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    arr_prev_centroids = (char **)calloc(k, sizeof(char*));

    if(!arr_prev_centroids){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    arr_clusters = (char ***)calloc(k, sizeof(char**));

    if(!arr_clusters){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    index_for_cluster_k = (int *)calloc(k, sizeof(int));

    if(!index_for_cluster_k){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    /*memset(buff, 0, sizeof(buff));*/

    for(f=0; f<data_num; f++){
        /*memset(data_points[k], 0, sizeof(data_points[k]));*/
        data_points[f] = (char *)calloc(line_len, sizeof(char));

        if(!data_points[f]){/*calloc failed*/
            printf("An Error Has Occurred\n");
            exit(1);
        }

        if(f<k){
            arr_centroids[f] = (char *)calloc(line_len, sizeof(char));

            if(!arr_centroids[f]){/*calloc failed*/
                printf("An Error Has Occurred\n");
                exit(1);
            }

            arr_prev_centroids[f] = (char *)calloc(line_len, sizeof(char));

            if(!arr_prev_centroids[f]){/*calloc failed*/
                printf("An Error Has Occurred\n");
                exit(1);
            }

            arr_clusters[f] = (char **)calloc(data_num, sizeof(char*));

            if(!arr_clusters[f]){/*calloc failed*/
                printf("An Error Has Occurred\n");
                exit(1);
            }
        }
    }

    for(f=0; f<k; f++){
        for(j =0; j<data_num; j++){
            arr_clusters[f][j] = (char *)calloc(line_len, sizeof(char));

            if(!arr_clusters[f][j]){/*calloc failed*/
                printf("An Error Has Occurred\n");
                exit(1);
            }

        }
    }
    
    fr = fopen(input_file, "r");

    if(fr == NULL){
        printf("Invalid Input!\n");
        exit(1);
    }

    i = 0;

    while(fscanf(fr, "%s", buff) == 1 ){/*copy each line in the input_file to data_points*/

        /*scanf(*(data_points+i*line_len), buff);*/
        strncpy(data_points[i], buff, line_len);/*maybe use strncpy ?*/
        i++;

    }/*end of while*/

    for(i=0; i<line_len+1; i++){
        /*count the numbers of commas, the dimension is the number
        of commas + 1*/
        if(data_points[0][i] == ','){
            dimension++;
        }/*end of if*/

    }/*end of for*/

    dimension++;

    /*printf("data_ point: %s\n", data_points[0]);*/

    fclose(fr);

    fw = fopen(output_file, "w");

    if(fw == NULL){
        printf("Invalid Input!\n");
        exit(1);
    }

    
    for(i=0; i<k; i++){/*initialization the centroids*/

        strncpy(arr_centroids[i], data_points[i], line_len);/*maybe use strncpy ?*/

        /*printf("the cent[%d]: %s\n", i, arr_centroids[i]);*/

    }/*end of for*/

    

    while((iter_num != max_iter) && !(euclidean_norm(k,  line_len, arr_centroids, arr_prev_centroids, dimension, iter_num))){
        
        
        
        for(m = 0; m<k; m++){/*nullify arr_clusters*/

            /*memset(arr_clusters[m], 0, sizeof(arr_clusters[m]));*/
            index_for_cluster_k[m] = 0;

        }/*end of for*/

        for(l = 0; l<k; l++){/*copy each vektor in arr_centroids to arr_prev_centroids for the euclidean_norm func*/

            strncpy(arr_prev_centroids[l], arr_centroids[l], line_len);/*maybe use strncpy ?*/

        }/*end of for*/
        
        /*printf("dat: %s\n", arr_centroids[0]);
        printf("dat: %s\n", arr_centroids[1]);
        printf("dat: %s\n", arr_centroids[2]);*/
        
        for(i=0; i<data_num; i++){/*after this for, in arr_cluster[i] there is all the x_j that u_i is their closest centroid*/

            
            index_to_cluster = calc_argmin(k, line_len, data_points[i], arr_centroids, dimension);

            strncpy(arr_clusters[index_to_cluster][index_for_cluster_k[index_to_cluster]], data_points[i], line_len );/*maybe use strncpy ?*/

            /*printf("this: %s\n", arr_clusters[index_to_cluster][index_for_cluster_k[index_to_cluster]]);*/
            index_for_cluster_k[index_to_cluster]++;

        }/*end of for*/

        /*printf("clust: %s\n", arr_clusters[0][0]);
        printf("clust: %s\n", arr_clusters[1][0]);
        printf("clust: %s\n", arr_clusters[2][0]);*/

        for(j=0; j<k; j++){/*this for is equal to the for in line 5 in the alg in ex1*/

            free(str_centroid_buffer);
            str_centroid_buffer = (char *)calloc(line_len, sizeof(char));

            if(!str_centroid_buffer){
                printf("An Error Has Occurred\n");
                exit(1);
            }

            /*memset(str_centroid_buffer, 0, sizeof(str_centroid_buffer));*/

            /*printf("this: %s\n", arr_clusters[j][0]);*/

            update_centroids(index_for_cluster_k[j], line_len, data_num, arr_clusters[j], dimension, str_centroid_buffer);

            strcpy(arr_centroids[j], str_centroid_buffer);/*maybe use strncpy ?*/
        }/*end of for*/
        /*and the writing to file after it*/

        /*printf("arr_prev_centroid: %s\n", arr_prev_centroids[1]);
        printf("arr_centroid: %s\n", arr_centroids[1]);*/
        iter_num++;/*increasing the number of iterations*/

    }/*end of while*/

    /*printf("number of iter: %d\n", iter_num);*/
    for(i=0; i<k; i++){/*write to file the centroids*/
        
        fprintf(fw, "%s\n", arr_centroids[i]);

    }/*end of for*/

    fclose(fw);

    free(buff);
    free(data_points);
    free(arr_centroids);
    free(arr_prev_centroids);
    free(arr_clusters);
    free(index_for_cluster_k);
    free(str_centroid_buffer);

    return 0;/*the run complete succesfuly*/

}/*end of kmeans function*/


int calcLineLen(char *input_file){
    FILE *fr = fopen(input_file, "r");
    int line_len = 0;
    int max_line = -1;

    if(fr == NULL){
        printf("Invalid Input!\n");
        exit(1);
    }

    while(!(feof(fr))){/*search the maximum line len */

        line_len = 0;

        while(fgetc(fr) != '\n'){

        line_len++;
        if(feof(fr)){
            break;
        }

        }/*end of inner while*/

        if(max_line < line_len){
            max_line = line_len;
        }

        /*if(feof(fr)){
            break;
        }*/

    }/*end of while*/

    max_line++;

    return max_line;

}/*end of function calcLineLen*/

/*this function return the numbers of non-white-lines under the assumption that in each line there is not white space*/
int numbersOfLines(char *input_file, int line_len){
    
    int num_of_lines = 0;

    char *buff;

    FILE *fr = fopen(input_file, "r");

    if(fr == NULL){
        printf("Invalid Input!\n");
        exit(1);
    }

    buff = (char *)calloc(line_len+1, sizeof(char));

    if(!buff){
        printf("An Error Has Occurred\n");
        exit(1);
    }

    while(fscanf(fr, "%s", buff ) == 1){/*count the numbers of lines*/
        
        num_of_lines++;

    }/*end of while*/

    free(buff);

    return num_of_lines;

    return -1;

}/*end of function numbersOfLines*/

int calc_argmin(int k, int line_len, const char *x, char **arr_centroids, int dimension){

     /*printf("centroid at start argmin: %s\n", arr_centroids[1]);*/
    double min_centr = calc_dist(x, arr_centroids[0], dimension, line_len);
    int index_cluster = 0;
    double tmp = min_centr + 1;
    int i;

    for(i=1; i<k; i++){/*the for calc (x-u_i)^2 when u_i == arr_centroids[i], and at the end min_centr == argmin(x-u_i)^2 and index_cluster == i*/
        /*printf("centroid send to calc_dist: %s\n", arr_centroids[i]);*/
        tmp = calc_dist(x, arr_centroids[i], dimension, line_len);

        if(tmp < min_centr){
            min_centr = tmp;
            index_cluster = i;
        }/*end of if*/

    }/*end of for*/

    return index_cluster;

    return -1;

}/*end of function calc_argmin*/

double calc_dist(const char *x, char *centroid, int dimension, int line_len){/*calculate (x-centroid)^2*/

    double *x_double;
    double *centroid_double;
    double tmp, sum;
    int i;
    sum = 0.0;
    tmp = 0.0;
    
    x_double = (double *)calloc(line_len, sizeof(double));

    if(!x_double){
        printf("An Error Has Occurred\n");
        exit(1);
    }

    centroid_double = (double *)calloc(line_len, sizeof(double));

    if(!centroid_double){
        printf("An Error Has Occurred\n");
        exit(1);
    }

    /*printf("to makeDataDuoblre: %s\n", centroid);*/

    /*memset(x_double, 0, sizeof(x_double));
    memset(centroid_double, 0, sizeof(centroid_double));*/

    makeDataDuoble(line_len, x, dimension, x_double);/*convert from string to double*/
    makeDataDuoble(line_len, centroid, dimension, centroid_double);/*convert from string to double*/

    for(i = 0; i<dimension; i++){/*calculate (x-centroid)^2 for each coordinate and sum them all*/

        tmp = x_double[i]-centroid_double[i];
        tmp = pow(tmp, 2);
        sum += tmp;

    }/*end of for*/

    free(x_double);
    free(centroid_double);

    return sum;

    return -1;

}/*end of function calc_dist*/


/*============================================================================
==============================================================================
== THIS WAS MY KMEANS IMPLEMENTATION, THE FUNCTIONS THEMSELVES NOT THE API! ==
============================ - END UP HERE - =================================
==============================================================================*/



/*
 * This actually defines the geo function using a wrapper C API function
 * The wrapping function needs a PyObject* self argument.
 * This is a requirement for all functions and methods in the C API.
 * It has input PyObject *args from Python.
 */
static PyObject* kmeans_capi(PyObject *self, PyObject *args)
{
    double z;
    int n;
    /* This parses the Python arguments into a double (d)  variable named z and int (i) variable named n*/
    if(!PyArg_ParseTuple(args, "di", &z, &n)) {
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }

/* This builds the answer ("d" = Convert a C double to a Python floating point number) back into a python object */
    return Py_BuildValue("d", geo_c(z, n)); /*  Py_BuildValue(...) returns a PyObject*  */


    /*THIS IS THE MAIN IN ASSIGNMENT 1, NEED TO DO THE SAME INSPECTIONS HERE*/
    /*before sending to the functions need to check validate of each argument*/
    
    /*return kmeans(3, 600, "input_1.txt", "output_1101_cc_001_.txt");*/


   /* if(argc == 4){
        /*printf("4 elements\n");
        if(atoi(argv[1]) == 0){
            printf("Invalid Input!\n");
            return 1;
        }
        return kmeans(atoi(argv[1]), DEFAULT_ITER, argv[2], argv[3]);
    }
    else if(argc == 5){
        /*printf("5 elements\n");
        if(atoi(argv[1]) == 0 || atoi(argv[2]) == 0){
            printf("Invalid Input!\n");
            return 1;
        }
        return kmeans(atoi(argv[1]), atoi(argv[2]), argv[3], argv[4]);
    }
    else{
        printf("Invalid Input!\n");
        return 1;
    }


    /*printf("%d", numbersOfLines("test line len.txt", 100));

    return 0;*/

}




/*
 * This array tells Python what methods this module has.
 * We will use it in the next structure
 */
static PyMethodDef capiMethods[] = {
    {"fit",                   /* the Python method name that will be used */
      (PyCFunction) kmeans_capi, /* the C-function that implements the Python function and returns static PyObject*  */
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