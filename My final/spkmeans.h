#ifndef SPKMEANS_H
#define SPKMEANS_H

int *mat_size(char *file_name);

double ** file_to_mat(char* filename);

double** wam_func(double **mat, int N, int dim);

double** ddg_func(double **mat, int N, int dim);

double** lnorm_func(double **mat, int N, int dim);

/*these 3 I need to add to the module*/
double **jacobi_func(double **A, int N);

int heuristic(double **mat, int N, int dim);

double** kmeans_double(int k, int max_iter, int eps, double** mat, int row, int dimension);

#endif

