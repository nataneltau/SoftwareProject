#ifndef SPKMEANS_H
#define SPKMEANS_H

int *mat_size(char *file_name);

double ** file_to_mat(char* filename);

double** wam_func(double **mat, int N, int dim);

double** ddg_func(double **mat, int N, int dim);

double** lnorm_func(double **mat, int N, int dim);

double **jacobi_func(double **A, int N);

int heuristic(double **mat, int N, int dim);

/*this I need to fix maybe in the module and add to the python file*/
int kmeans_double(int k, int max_iter, double eps, double** mat, int row, int dimension, double **first_index);

double **vectors_matrix(double **mat, int N, int k);

#endif

