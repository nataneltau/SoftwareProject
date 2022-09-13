#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "spkmeans.h" /*our library*/
/* check that we free al unnecessary memory */


/*ENUM*/
enum goal{wam, ddg, lnorm, jacobi};
/*ENUM*/

/*============================================================================
==============================================================================
== THIS IS MY KMEANS IMPLEMENTATION, THE FUNCTIONS THEMSELVES NOT THE API! ===
============================ - START DOWN HERE - =============================
==============================================================================*/


const int DEFAULT_ITER = 200;
double epsi;


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

double calc_dist_double(const double *x, double *centroid, int dimension);/*this function calculate (x-centroid)^2*/

int calc_argmin_double(int k, const double *x, double **arr_centroids, int dimension);/*return index j for S_j: argmin*/

int calcLineLen(char *input_file);/*calculate the maximum line len*/

int numbersOfLines(char *input_file, int line_len);/*return the numbers of lines in the file*/

/*let's build some functions*/
int euclidean_norm_double( int k, double **arr_centroids, double **arr_prev_centroids, int dimension, int iter_num){/*check condition to while loop in kmeans function*/

    double tmp;
    int i;
    double small_than_e;
    small_than_e = epsi;

    if(iter_num == 0){/*we are at the first iteration, return false*/
        return 0;
    }


    for(i = 0; i<k; i++){/*each iteration check if there is a centroid which his euclidean norm bigger than 0.001*/

        tmp = calc_dist_double(arr_prev_centroids[i], arr_centroids[i], dimension);
        tmp = fabs(tmp);
        tmp = sqrt(tmp);

        if(tmp >= small_than_e){/*if this true than there is at least one centroid which his euclidean norm bigger than 0.001*/
            return 0;
        }/*end of if*/


    }/*end of for*/
    
    return 1;
}/*end of function euclidean_norm*/

/*this func calculate the new centroids, like line6 in the alg in ex1*/
void update_centroids(double clus_len, double **cluster, int dimension, double *arr_centroid){

    double *the_new_centroid;
    int i, j, n;

    the_new_centroid = (double *)calloc(dimension, sizeof(double));

    if(!the_new_centroid){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    if(clus_len == 0){/*cluster to this centroid is empty, never should get in this block*/
        printf("An Error Has Occurred\n");
        exit(1);
    }/*end of if*/

    for(i=0; i<clus_len; i++){/*sum each coordinate for all the vektors in the cluster*/

        for(j=0; j<dimension; j++){

            the_new_centroid[j] += cluster[i][j];

        }/*end of inner for*/

    }/*end of for*/


    for(n=0; n<dimension; n++){/*normalization*/
        the_new_centroid[n] = the_new_centroid[n] / clus_len;
    }/*end of for*/

    memcpy(arr_centroid, the_new_centroid, dimension*sizeof(double));
    
    free(the_new_centroid);

}/*end of function update_centroids*/

/*tested on input, worked right*/
int kmeans_double(int k, int max_iter, double eps, double** mat, int row, int dimension, double **first_index){/*should write the k wanted centroid at the start of the file*/

    int iter_num = 0;
    double *buff;
    double **arr_centroids;
    double **arr_prev_centroids;
    double ***arr_clusters;
    int *index_for_cluster_k;
    int f, j, i, m, l;
    int index_to_cluster;
    double *str_centroid_buffer;
    double *the_index_of_cent;

    /*
        I replaced data_num in rows,
            line_len in dimension,
            data_points in mat
    
    */


    epsi = eps;


    if(row <= 0 || dimension <= 0 || k <= 0 || max_iter <= 0){
        printf("Invalid Input!\n");
        exit(1);
    }

    str_centroid_buffer = (double *)calloc(dimension, sizeof(double));

    if(!str_centroid_buffer){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    buff = (double *)calloc(dimension, sizeof(double));

    if(!buff){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    arr_centroids = (double **)calloc(k, sizeof(double*));

    if(!arr_centroids){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    the_index_of_cent = (double *)calloc(k, sizeof(double*));

    if(!the_index_of_cent){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    arr_prev_centroids = (double **)calloc(k, sizeof(double*));

    if(!arr_prev_centroids){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    arr_clusters = (double ***)calloc(k, sizeof(double**));

    if(!arr_clusters){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    index_for_cluster_k = (int *)malloc(k *sizeof(int));

    if(!index_for_cluster_k){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    /*memset(buff, 0, sizeof(buff));*/

    for(f=0; f<k; f++){
        /*memset(data_points[k], 0, sizeof(data_points[k]));*/
          
            arr_centroids[f] = (double *)calloc(dimension, sizeof(double));

            if(!arr_centroids[f]){/*calloc failed*/
                printf("An Error Has Occurred\n");
                exit(1);
            }

            arr_prev_centroids[f] = (double *)calloc(dimension, sizeof(double));

            if(!arr_prev_centroids[f]){/*calloc failed*/
                printf("An Error Has Occurred\n");
                exit(1);
            }

            arr_clusters[f] = (double **)calloc(row, sizeof(double*));

            if(!arr_clusters[f]){/*calloc failed*/
                printf("An Error Has Occurred\n");
                exit(1);
            }
        
    }

    for(f=0; f<k; f++){
        for(j =0; j<row; j++){
            arr_clusters[f][j] = (double *)calloc(dimension, sizeof(double));

            if(!arr_clusters[f][j]){/*calloc failed*/
                printf("An Error Has Occurred\n");
                exit(1);
            }

        }
    }
    

    for(i=0; i<k; i++){

        memcpy(arr_centroids[i], mat[(int)first_index[0][i]], dimension * sizeof(double));
    }


    while((iter_num != max_iter) && !(euclidean_norm_double(k, arr_centroids, arr_prev_centroids, dimension, iter_num))){
        
        
        
        for(m = 0; m<k; m++){/*nullify arr_clusters*/

            /*memset(arr_clusters[m], 0, sizeof(arr_clusters[m]));*/
            index_for_cluster_k[m] = 0;

        }/*end of for*/

        for(l = 0; l<k; l++){/*copy each vektor in arr_centroids to arr_prev_centroids for the euclidean_norm func*/

            memcpy(arr_prev_centroids[l], arr_centroids[l], dimension * sizeof(double));

        }/*end of for*/
        
        /*printf("dat: %s\n", arr_centroids[0]);
        printf("dat: %s\n", arr_centroids[1]);
        printf("dat: %s\n", arr_centroids[2]);*/
   
        for(i=0; i<row; i++){/*after this for, in arr_cluster[i] there is all the x_j that u_i is their closest centroid*/

            
            index_to_cluster = calc_argmin_double(k, mat[i], arr_centroids, dimension);

            memcpy(arr_clusters[index_to_cluster][index_for_cluster_k[index_to_cluster]], mat[i], dimension * sizeof(double) );

            /*printf("this: %s\n", arr_clusters[index_to_cluster][index_for_cluster_k[index_to_cluster]]);*/
            index_for_cluster_k[index_to_cluster]++;

        }/*end of for*/

        /*printf("clust: %s\n", arr_clusters[0][0]);
        printf("clust: %s\n", arr_clusters[1][0]);
        printf("clust: %s\n", arr_clusters[2][0]);*/

        for(j=0; j<k; j++){/*this for is equal to the for in line 5 in the alg in ex1*/

            free(str_centroid_buffer);
            str_centroid_buffer = (double *)calloc(dimension, sizeof(double));

            if(!str_centroid_buffer){
                printf("An Error Has Occurred\n");
                exit(1);
            }

            /*memset(str_centroid_buffer, 0, sizeof(str_centroid_buffer));*/

            /*printf("this: %s\n", arr_clusters[j][0]);*/

            update_centroids(index_for_cluster_k[j], arr_clusters[j], dimension, arr_centroids[j]);

        }/*end of for*/
        /*and the writing to file after it*/

        /*printf("arr_prev_centroid: %s\n", arr_prev_centroids[1]);
        printf("arr_centroid: %s\n", arr_centroids[1]);*/
        iter_num++;/*increasing the number of iterations*/

    }/*end of while*/



    /*printf("number of iter: %d\n", iter_num);*/

    for(i=0; i<k; i++){/*write to file the centroids*/
        
        for(j=0; j<dimension; j++){

            fprintf(stdout, "%.4f, ", arr_centroids[i][j]);
        }

        fprintf(stdout, "%.4f\n", arr_centroids[i][dimension-1]);

    }/*end of for*/

    for(f=0; f<k; f++){
        for(j =0; j<row; j++){
            free(arr_clusters[f][j]);

        }
    }


    for(f=0; f<k; f++){
        /*memset(data_points[k], 0, sizeof(data_points[k]));*/

        free(arr_centroids[f]);

        free(arr_prev_centroids[f]);

        free(arr_clusters[f]);

        
    }



    free(buff);
    free(arr_centroids);
    free(arr_prev_centroids);
    free(arr_clusters);
    free(index_for_cluster_k);
    free(str_centroid_buffer);
    free(the_index_of_cent);

    return 0;/*the run complete succesfuly*/

}/*end of kmeans_double function*/


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

    fclose(fr);

    return max_line;

}/*end of function calcLineLen*/

/*this function return the numbers of non-white-lines under the assumption that in each line there is not white space*/
int numbersOfLines(char *input_file, int line_len){
    
    int num_of_lines = 0;

    char *buff;
    /*printf("we start numbersOfLines\n");*/
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

    fclose(fr);

    return num_of_lines;

    return -1;

}/*end of function numbersOfLines*/

int calc_argmin_double(int k, const double *x, double **arr_centroids, int dimension){

     /*printf("centroid at start argmin: %s\n", arr_centroids[1]);*/
    double min_centr = calc_dist_double(x, arr_centroids[0], dimension);
    int index_cluster = 0;
    double tmp = min_centr + 1;
    int i;

    for(i=1; i<k; i++){/*the for calc (x-u_i)^2 when u_i == arr_centroids[i], and at the end min_centr == argmin(x-u_i)^2 and index_cluster == i*/
        /*printf("centroid send to calc_dist: %s\n", arr_centroids[i]);*/
        tmp = calc_dist_double(x, arr_centroids[i], dimension);

        if(tmp < min_centr){
            min_centr = tmp;
            index_cluster = i;
        }/*end of if*/

    }/*end of for*/

    return index_cluster;

    return -1;

}/*end of function calc_argmin*/

double calc_dist_double(const double *x, double *centroid, int dimension){/*calculate (x-centroid)^2*/

    double tmp, sum;
    int i;
    sum = 0.0;
    tmp = 0.0;
    
   
    /*printf("to makeDataDuoblre: %s\n", centroid);*/

    /*memset(x_double, 0, sizeof(x_double));
    memset(centroid_double, 0, sizeof(centroid_double));*/

    for(i = 0; i<dimension; i++){/*calculate (x-centroid)^2 for each coordinate and sum them all*/

        tmp = x[i]-centroid[i];
        tmp = pow(tmp, 2);
        sum += tmp;

    }

    return sum;

    return -1;

}/*end of function calc_dist*/


/*============================================================================
==============================================================================
== THIS WAS MY KMEANS IMPLEMENTATION, THE FUNCTIONS THEMSELVES NOT THE API! ==
============================ - END UP HERE - =================================
=============================================================================*/


static int compare (const void * a, const void * b)
{
  if (*(double*)a > *(double*)b) return -1;
  else if (*(double*)a < *(double*)b) return 1;
  else return 0;
}

int find_k(double **mat, int N){
    double *delta, *eigenvalues;
    double max_val;
    int res, i;
    eigenvalues = mat[0];
    qsort(eigenvalues, N, sizeof(double), compare);
    printf("eigenvalues: \n");
    for (i = 0; i < N; i++){
        printf("%f, ", eigenvalues[i]);
    }
    printf("\n");



    delta = calloc(N-1, sizeof(double));
    for (i = 0; i < N-1; i++){
        delta[i] = fabs(eigenvalues[i] - eigenvalues[i+1]);
    }

    max_val = delta[0];
    res = 0;
    for (i = 0; i < N/2; i++){
        if (delta[i] > max_val){
            max_val = delta[i];
            res = i;
        }
    }
    return res + 1;
}

double **new_mat(int N){
    double **mat;
    int i;
    mat = calloc(N, sizeof(double*));
    if(!mat){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    for(i=0; i<N; i++){
        mat[i] = calloc(N, sizeof(double));
        if(!mat){/*calloc failed*/
            printf("An Error Has Occurred\n");
            exit(1);
        }
    }
    return mat;
}

double **I_mat(int N){
    double **mat;
    int i;
    mat = new_mat(N);
    for (i = 0; i < N; i++){
        mat[i][i] = 1;
    }
    return mat;
}

double *get_eigen(double **mat, int N){
    double *eignvalues;
    int i;
    eignvalues = calloc(N, sizeof(double));
    for (i = 0; i < N; i++){
        eignvalues[i] = mat[i][i];
    }
    return eignvalues;
}


double *test_ctopy(){
    double *lst;
    lst = calloc(4, sizeof(double));
    return lst;
}



int *mat_size(char *file_name){
    int *res, rows, columns, i;
    char *line;
    int line_len;
    FILE *fr;
    rows = 0;
    columns = 0;
    line_len = calcLineLen(file_name);
    line = (char *)calloc( line_len, sizeof(char));
    fr = fopen(file_name, "r");
    if(fr == NULL){
        printf("Invalid Input!\n");
        exit(1);
    }
    fgets(line, line_len, fr);
    rows++;
    i = 0;
    while(line[i] != '\0'){
        if (line[i] == ','){
            columns++;
        }
        i++;
    }
    columns++;


    while (fgets(line, line_len, fr) != NULL) {
        rows += 1;
    }

    fclose(fr);
    rows = numbersOfLines(file_name, line_len);

    res = calloc(2, sizeof(int));

    res[0] = rows;
    res[1] = columns;
    /*printf("rows: %i\n", rows);
    printf("columns: %i\n", columns);*/
    return res;
}

double **get_mat(char *file_name, int rows, int columns){
    double **mat;
    int i, j;
    double temp;
    char /*line[(columns+1)*sizeof(double)],*/ *token;
    char line[250];
    FILE *fr;

    /*max_chars_in_line = calcLineLen( file_name);
    line = (char *)malloc(max_chars_in_line * sizeof(char));*/
    mat = (double **)malloc(rows * sizeof(double*));
    if(!mat){/*malloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    for(i=0; i < rows; i++){
        mat[i] = (double *)malloc(columns * sizeof(double));
        if(!mat[i]){/*malloc failed*/
            printf("An Error Has Occurred\n");
            exit(1);
        }
    }

    fr = fopen(file_name, "r");
    if(fr == NULL){
        printf("Invalid Input!\n");
        exit(1);
    }
    i = 0;

    while (fgets(line, (columns+1)*sizeof(double), fr) != NULL && i<rows) {
        j = 0;
        token = strtok(line, ",");

        while (token != NULL && j<columns){
            temp = strtod(token, NULL);
            mat[i][j] = temp;
            token = strtok(NULL, ",");
            j++;
        }
        i++;

    }

    /*free(line);*/

    return mat;



}

void free_mat(double **mat, int m){
    int row;
    for (row = 0; row < m; row++){
        free(mat[row]);
    }
    free(mat);
}

double calc_dist(double *x, double *y, int dim){/*calculate (x-y)^2*/


    double tmp, sum;
    int i;
    sum = 0.0;
    tmp = 0.0;



    for(i = 0; i<dim; i++){/*calculate (x-centroid)^2 for each coordinate and sum them all*/

        tmp = x[i]-y[i];
        tmp = pow(tmp, 2);
        sum += tmp;

    }/*end of for*/


    return sum;

    /*return -1;*/

}/*end of function calc_dist*/

double calc_exp_euc(double* x, double* y, int dim){
    double euc, res;

    euc = calc_dist(x, y, dim);
    res = sqrt(fabs(euc));
    res = exp(res*(-1.0/2.0));

    return res;
}/*end of function calc_exp_euc*/

double** wam_func(double **mat, int N, int dim){/*tested on input, worked right*/
    double** wei_adj_mat;
    int i, j;

    wei_adj_mat = (double **)malloc(N * sizeof(double*));
    if(!wei_adj_mat){/*malloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    for(i=0; i<N; i++){
        wei_adj_mat[i] = (double *)malloc(N * sizeof(double));
        if(!wei_adj_mat){/*malloc failed*/
            printf("An Error Has Occurred\n");
            exit(1);
        }
    }

    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            if (i==j){
                wei_adj_mat[i][j] = 0;
            }
            else if(i > j){
                wei_adj_mat[i][j] = wei_adj_mat[j][i];
            }
            else{
                wei_adj_mat[i][j] = calc_exp_euc(mat[i], mat[j], dim);
            }
        }
    }

    return wei_adj_mat;

}/*end of function wam_func*/

double sum_line(double* line, int N){
    int i;
    double sum;
    sum = 0.0;

    for(i=0; i<N; i++){
        sum += line[i];
    }

    return sum;
}


double** ddg_func(double **mat, int N, int dim){/*tested on imput, works right*/
    double** dia_deg_mat,** wei_adj_mat;
    int i, j;

    dia_deg_mat = (double **)malloc(N * sizeof(double*));
    if(!dia_deg_mat){
        printf("An Error Has Occurred\n");
        exit(1);
    }

    for(i=0; i<N; i++){
        dia_deg_mat[i] = (double *)malloc(N * sizeof(double));
        if(!dia_deg_mat){
            printf("An Error Has Occurred\n");
            exit(1);
        }
    }

    wei_adj_mat = wam_func(mat, N, dim);

    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            if (i != j){
                dia_deg_mat[i][j] = 0;
            }
            else{
                dia_deg_mat[i][j] = sum_line(wei_adj_mat[i], N);
            }
        }
    }
    free_mat(wei_adj_mat, N);
    return dia_deg_mat;
}/*end of function ddg_func*/

double** calc_mat_sqrt(double **dia_deg_mat, int N){
    double **dia_deg_mat_sqrt;
    int i, j;

    dia_deg_mat_sqrt = (double **)malloc(N * sizeof(double*));
    if(!dia_deg_mat_sqrt){/*malloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    for(i=0; i<N; i++){
        dia_deg_mat_sqrt[i] = (double *)malloc(N * sizeof(double));
        if(!dia_deg_mat_sqrt){/*malloc failed*/
            printf("An Error Has Occurred\n");
            exit(1);
        }
    }

    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
                if(j!=i){
                    dia_deg_mat_sqrt[i][j] = 0;
                }
                else{/* i == j */
                    dia_deg_mat_sqrt[i][j] = 1.0/sqrt(dia_deg_mat[i][j]);
                }

        }
    }

    return dia_deg_mat_sqrt;
}

double** mat_mult(double** mat1, double** mat2, int N){
    int i,j,k;
    double** rslt;

    rslt = (double **)malloc(N * sizeof(double*));
    if(!rslt){/*malloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    for(i=0; i<N; i++){
        rslt[i] = (double *)malloc(N * sizeof(double));
        if(!rslt){/*malloc failed*/
            printf("An Error Has Occurred\n");
            exit(1);
        }
    }


     for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            rslt[i][j] = 0;

            for (k = 0; k < N; k++) {

                rslt[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
     }
     return rslt;
}

double **mat_mult3(double **mat1, double ** mat2, double **mat3, int N){
    double **res, **mult1;

    mult1 = mat_mult(mat1, mat2, N);
    res = mat_mult(mult1, mat3, N);

    return res;

}

double** lnorm_func(double **mat, int N, int dim){/*tested on input, works right*/
    double** dia_deg_mat,** wei_adj_mat,** norm;
    double ** mult1,** mult2,** dia_deg_sqrt_mat;
    int i, j;

    dia_deg_mat = ddg_func(mat, N, dim);
    wei_adj_mat = wam_func(mat, N, dim);
    dia_deg_sqrt_mat = calc_mat_sqrt(dia_deg_mat, N);

    norm = (double **)malloc(N * sizeof(double*));
    if(!norm){
        printf("An Error Has Occurred\n");
        exit(1);
    }

    for(i=0; i<N; i++){
        norm[i] = (double *)malloc(N * sizeof(double));
        if(!norm){
            printf("An Error Has Occurred\n");
            exit(1);
        }
    }

    mult1 = mat_mult(dia_deg_sqrt_mat, wei_adj_mat, N);
    mult2 = mat_mult(mult1, dia_deg_sqrt_mat, N);

    for(i=0; i<N; i++){

            for(j=0; j<N; j++){
                if(i==j){
                    norm[i][j] = 1 - mult2[i][j];
                }
                else{
                    norm[i][j] = 0 - mult2[i][j];
                }
            }
        }

    free(dia_deg_mat);
    free(wei_adj_mat);
    free(mult1);
    free(mult2);
    free(dia_deg_sqrt_mat);

    return norm;


}

int sign(double num){
    if (num >= 0){
        return 1;
    }
    return -1;
}

int *find_pivot(double **mat, int N){  /* mat is NxN */
    int i, j, row, column;
    double max_val = mat[0][1];
    int *res = calloc(2, sizeof(int));
    i = 0;
    j = 1;
    for (row = 0; row < N; row++){
        for (column = 0; column < N; column++){
            if (row != column && fabs(mat[row][column]) >= max_val){
                max_val = fabs(mat[row][column]);
                i = row;
                j = column;
            }
        }
    }
    res[0] = i;
    res[1] = j;
    return res;
}


double **Rotation_mat(double **S, int N){  /*returns P the Jacobi rotation matrix of S*/
    double c, s, teta, t, A_ii, A_jj, A_ij;
    double **P;
    int *pivot;
    int row, column, i, j;

    pivot = find_pivot(S, N);

    P = calloc(N, sizeof(double*));
    for (row = 0; row < N; row++){
        P[row] = calloc(N, sizeof(double));
    }

    A_jj = S[pivot[1]][pivot[1]];
    A_ii = S[pivot[0]][pivot[0]];
    A_ij = S[pivot[0]][pivot[1]];
    teta = (A_jj-A_ii)/(2*A_ij);

    t = sign(teta)/(fabs(teta) + sqrt(teta*teta + 1));
    c = 1/sqrt(t*t + 1);
    s = t*c;



    /*i = fmin(pivot[0], pivot[1]);
    j = fmax(pivot[0], pivot[1]);*/
    i = pivot[0];
    j = pivot[1];
    for (row = 0; row < N; row++){
        for (column = 0; column < N; column++){
            if (row == column && row != pivot[0] && row != pivot[1]){ /* case of 1 */
                P[row][column] = 1;
            }
            if (((row == column) && (row == pivot[0]))|| ((row == column) && (row == pivot[1]))){    /* case of c*/
                P[row][column] = c;
            }
            if (row == i && column == j){/*N-1-i*/  /*case of s  might not be max min*/
                P[row][column] = s;
            }
            if (row == j && column == i){/*N-1-j*/   /*case of -s    might not be max min*/
                P[row][column] = -1*s;
            }

        }
    }
    return P;

}

double **transpose(double **mat, int N){
    double **transposed;
    int row, column;
    transposed = calloc(N, sizeof(double*));
    for (row = 0; row < N; row++){
        transposed[row] = calloc(N, sizeof(double));
    }
    for (row = 0; row < N; row++){
        for (column = 0; column < N; column++){
            transposed[row][column] = mat[column][row];
        }
    }
    return transposed;

}



double off(double **mat, int N){
    double res;
    int row, column;

    res = 0;

    for (row = 0; row < N; row++){
        for (column = 0; column < N; column++){
            if (row != column){
                res += mat[row][column] * mat[row][column];
            }
        }
    }
    return res;
}

void print_mat(double **mat, int N){
    int row, column;
    for (row = 0; row < N; row++){
        for (column = 0; column < N-1; column++){
            printf("%f, ", mat[row][column]);
        }
        printf("%f\n", mat[row][N-1]);
    }
}



double **jacobi_func(double **A, int N){/*tested on input, works right*/
    double eps, dif, offA, offB;
    double **P, **P_t, **B, **V, **res;
    double *eigenvalues;
    int counter, i;


    eps = 1.0 * pow(10, -5);
    counter = 0;
    dif = 10;
    offA = off(A, N);
    V = I_mat(N);

    while (dif > eps && counter < 100){
        P = Rotation_mat(A, N);
        V = mat_mult(V, P, N);
        P_t = transpose(P, N);
        B = mat_mult3(P_t, A, P, N);/* it's A' */
        offB = off(B, N);
        dif = fabs(offA - offB);
        free_mat(A, N);
        A = B;
        offA = offB;
        counter += 1;
    }
    eigenvalues = get_eigen(A, N);
    res = calloc(N+1, sizeof(double*));
    for (i = 0; i < N+1; i++){
        res[i] = calloc(N, sizeof(double));
    }
    res[0] = eigenvalues;
    for (i = 0; i < N; i++){
        res[i+1] = V[i];
    }
    free_mat(V, N);

    return res;

}

int calc_largest_vec(double **mat, int N){

     /*printf("centroid at start argmin: %s\n", arr_centroids[1]);*/
    double *zero_vec;
    double max_centr; 
    int index_cluster = 1;
    double tmp;
    int i;

    zero_vec = (double *)calloc(N, sizeof(double));/*make vectors of zeroes*/

    if(!zero_vec){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    max_centr = calc_dist_double(zero_vec, mat[1], N);
    tmp = max_centr + 1;

    for(i=2; i<N+1; i++){/*the for calc (x-u_i)^2 when u_i == arr_centroids[i], and at the end min_centr == argmin(x-u_i)^2 and index_cluster == i*/
        /*printf("centroid send to calc_dist: %s\n", arr_centroids[i]);*/
        tmp = calc_dist_double(zero_vec, mat[i], N);

        if(tmp > max_centr){
            max_centr = tmp;
            index_cluster = i;
        }/*end of if*/

    }/*end of for*/

    return index_cluster;

    return -1;

}/*end of function calc_argmin*/

/*this function get jacobi matrix*/
double **vectors_matrix(double **mat, int N, int k){/*do step 4-5 in algorithm 1*/

    double calc_dist_double(const double *x, double *centroid, int dimension);/*this function calculate (x-centroid)^2*/

    double **vec_mat;
    int i, j;
    int index, tmp, sum;

    vec_mat = (double **)calloc(N, sizeof(double *));

    if(!vec_mat){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    for(i=0; i<N; i++){
        vec_mat[i] = (double *)calloc(k, sizeof(double));

        if(!vec_mat[i]){/*calloc failed*/
            printf("An Error Has Occurred\n");
            exit(1);
        }
    }/*end of for*/

    for(i = 0; i<k; i++){

        /*choose the first eigenvectors*/
        /*tmp = mat[0][0];
        index = 0;

        for(j=1; j<N; j++){

        }*/

        index = calc_largest_vec(mat, N);

        for(j=0; j<N; j++){

            vec_mat[j][i] = mat[index+1][j]; /*letting the largest vector as column*/

        }/*end of inner for*/

        for(j=0; j<N; j++){

            mat[index+1][j] = 0; /*make the largest full of zeroes*/

        }/*end of inner for*/

    }/*end of outer for*/

    /*step 5 of alg, renormalizing each of vec_mat rows*/

    for(i=0; i<N; i++){

        sum =0;

        for(j=0; j<k; j++){

            tmp = vec_mat[i][j];
            tmp = pow(tmp, 2);
            sum += tmp;


        }/*end of inner for*/

        sum = sqrt(sum);

        for(j=0; j<k; j++){

            vec_mat[i][j] /= sum;

        }/*end of inner for*/


    }/*end of outer for*/

    return vec_mat;

}/*end of vectors_matrix function*/

int heuristic(double **mat, int N, int dim){
    double **lnorm_mat, **jacobi_mat;
    int k;

    lnorm_mat = lnorm_func(mat, N, dim);
    printf("finished lnorm_func in heuristic\n");
    jacobi_mat = jacobi_func(lnorm_mat, N);
    printf("finished jacobi_func in heuristic\n");
    k = find_k(jacobi_mat, N);
    return k;
}

/*double **spk(double **mat, int N, int dim, int k, int max_iter, int eps){
    double **res;
    printf("k = %i",k);

    res = calloc(2, sizeof(double*));
    res[0] = calloc(2, sizeof(double));
    res[1] = calloc(2, sizeof(double));
    return res;
}*/


void test_jacoby(){
    double **mat, **A;
    int *size;
    char *file_name;

    file_name = "jacobi_test.txt";
    size = mat_size(file_name);
    mat = file_to_mat(file_name);

    printf("original mat:\n");
    print_mat(mat, size[0]);

    A = jacobi_func(mat, size[0]);
    printf("Jacobi res:\n");
    print_mat(A, size[0]+1);
}

double **test_pytoc(double **mat, int N){
    print_mat(mat, N);
    /*free_mat(mat, N);*/
    return mat;
}

void test_wam(){
    double **mat, **A;
    mat = calloc(2, sizeof(double*));
    mat[0] = calloc(2, sizeof(double));
    mat[1] = calloc(2, sizeof(double));
    mat[0][0] = 1;
    mat[0][1] = 2;
    mat[1][0] = 3;
    mat[1][1] = 4;
    printf("wam_func:\n");
    A = wam_func(mat, 2, 2);
    print_mat(A, 2);
    free_mat(A, 2);
    free_mat(mat, 2);
}

/*
void test_ddg(){
    double **mat, **A, **B;
    int i, j;
    mat = calloc(3, sizeof(double*));
    mat[0] = calloc(3, sizeof(double));
    mat[1] = calloc(3, sizeof(double));
    mat[2] = calloc(3, sizeof(double));
    for (i = 0; i < 3; i++){
        for (j = 0; j<3; j++){
            mat[i][j] = i*3+j;
        }
    }
    printf("ddg_func:\n");
    B = ddg_func(mat, 3);
    print_mat(B, 3);
    printf("wam_func:\n");
    A = wam_func(mat, 3);
    print_mat(A, 3);
    free_mat(B, 3);
}
*/
/*
void test_lnorm(){
    double **mat, **A;
    int i, j;
    mat = calloc(3, sizeof(double*));
    mat[0] = calloc(3, sizeof(double));
    mat[1] = calloc(3, sizeof(double));
    mat[2] = calloc(3, sizeof(double));
    for (i = 0; i < 3; i++){
        for (j = 0; j<3; j++){
            mat[i][j] = i*3+j;
        }
    }

    A = lnorm_func(mat, 3);
    print_mat(A, 3);
    free_mat(A, 3);
    free_mat(mat, 3);
}
*/

/*int numbersOfLines(char *input_file, int line_len){

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

    count the numbers of lines
    while(fscanf(fr, "%s", buff ) == 1){

        num_of_lines++;

    }end of while*/

    /*free(buff);

    return num_of_lines;

    return -1;
    
}*/
/*end of function numbersOfLines*/


/*Tested, works right, read the file and make double matrix from it*/
double ** file_to_mat(char* filename){
    FILE *fr;
    double **res_mat;
    char *buff;
    char **data_points;
    int i, f;
    int dimension = 0;
    int line_len = calcLineLen(filename);
    int data_num = numbersOfLines(filename, line_len);
    /*int data_num = mat_size(filename)[0];*/

    data_points = (char **)calloc(data_num, sizeof(char*));

    if(!data_points){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    buff = (char *)calloc(line_len, sizeof(char));

    if(!buff){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    for(f=0; f<data_num; f++){
        /*memset(data_points[k], 0, sizeof(data_points[k]));*/
        data_points[f] = (char *)calloc(line_len, sizeof(char));

    }/*end of for*/

    fr = fopen(filename, "r");

    if(fr == NULL){
        printf("Invalid Input!\n");
        exit(1);
    }

    i=0;

    while(fscanf(fr, "%s", buff) == 1 && i<data_num){/*copy each line in the input_file to data_points*/

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

    /*row_and_col = mat_size(filename);

    int num_of_cord = row_and_col[1];
    int data_num = row_and_col[0];*/

    /*res_mat = get_mat(filename, data_num, num_of_cord);*/

    res_mat = (double **)calloc(data_num, sizeof(double*));

    for(i=0; i<data_num; i++){

        res_mat[i] = (double *)calloc(dimension, sizeof(double));

        if(!res_mat[i]){/*calloc failed*/
            printf("An Error Has Occurred\n");
            exit(1);
        }

        makeDataDuoble(line_len, data_points[i], dimension, res_mat[i]);

    }

    return res_mat;
}

void print_mat_normal(double **mat, int row, int col){
    int i, j;
    for (i = 0; i < row; i++){
        for (j = 0; j < col-1; j++){
            printf("%.4f,", mat[i][j]);
        }
        printf("%.4f\n", mat[i][col-1]);
    }
}



int main(int argc, char *argv[]){

    /*before sending to the functions need to check validate of each argument*/
    /*in lnorm and ddg, maybe also wam and jacobi, there is inaccuracies like 0.0006, check
        if it something needed fix*/

    int j;
    char* goal; 
    char *file_name;
    int *size;
    double **mat, **mat_to_print;

    if(argc == 3){
        
        file_name = argv[2];
        size = mat_size(file_name);/*size[0] is row, size[1] is col*/
        mat = file_to_mat(file_name);
        goal = argv[1];/*the enum*/


        if (strcmp(goal, "wam") == 0) {
            mat_to_print = wam_func(mat, size[0], size[1]);
        }
        else if (strcmp(goal, "ddg") == 0) {
            mat_to_print = ddg_func(mat, size[0], size[1]);
        }
        else if (strcmp(goal, "lnorm") == 0) {
            mat_to_print = lnorm_func(mat, size[0], size[1]);
        }
        else if (strcmp(goal, "jacobi") == 0) {
            mat_to_print = jacobi_func(mat, size[0]);
        }
        else {
            printf("Invalid Input!\n");
            exit(1);
        }

        print_mat_normal(mat_to_print, size[0], size[0]);/*the mat is square matrix*/

        if (strcmp(goal, "jacobi") == 0) {/*jacobi is (Nx1)xN*/
            for (j = 0; j < size[0]-1; j++){
                printf("%.4f,", mat_to_print[size[0]][j]);
            }
            printf("%.4f\n", mat_to_print[size[0]][size[0]-1]);
        }
        
    }/*end of if*/
    else{
        printf("Invalid Input!\n");
        return 1;
    }


    /*char *file_name;
    int *size, k;
    double **mat, **jacobi_mat, **wam_mat, **ddg_mat, **ddg_sqrt, **lnorm_mat;
    file_name = "tmpFile.txt";
    size = mat_size(file_name);
    mat = file_to_mat(file_name);
    

    wam_mat = wam_func(mat, size[0], size[1]);
    ddg_mat = ddg_func(mat, size[0], size[1]);
    ddg_sqrt = calc_mat_sqrt(ddg_mat, size[0]);
    lnorm_mat = lnorm_func(mat, size[0], size[1]);

    print_mat_normal(mat, size[0], size[1]);

    printf("wam_mat: \n");
    print_mat_normal(wam_mat, size[0], size[0]);

    printf("\n");

    printf("ddg_mat: \n");
    print_mat_normal(ddg_mat, size[0], size[0]);

    printf("\n");

    printf("ddg_sqrt: \n");
    print_mat_normal(ddg_sqrt, size[0], size[0]);

    printf("\n");

    printf("lnorm_mat: \n");
    print_mat_normal(lnorm_mat, size[0], size[0]);

    printf("\n");
    */


    /*return kmeans(3, 600, "input_1.txt", "output_1101_cc_001_.txt");*/
    /*




    char *file_name;
    int *size, k;
    double **mat, **jacobi_mat, **wam_mat, **ddg_mat, **ddg_sqrt, **lnorm_mat;
    file_name = "tmpFile.txt";
    size = mat_size(file_name);
    mat = get_mat(file_name, size[0], size[1]);
    mat = file_to_mat(file_name);
    print_mat_normal(mat, size[0], size[1]);
    kmeans_double(3, 600, 0.1, mat, size[0], size[1]);*/

    /*wam_mat = wam_func(mat, size[0], size[1]);
    ddg_mat = ddg_func(mat, size[0], size[1]);
    ddg_sqrt = calc_mat_sqrt(ddg_mat, size[0]);
    lnorm_mat = lnorm_func(mat, size[0], size[1]);
    k = heuristic(mat, size[0], size[1]);*/

    /*printf("k = %d", k);
    test_wam();*/

    /*jacobi_mat = jacobi_func(mat, size[0]);
    printf("wam_mat: \n");
    print_mat(wam_mat, size[0]);

    printf("\n");

    printf("ddg_mat: \n");
    print_mat(ddg_mat, size[0]);

    printf("\n");

    printf("ddg_sqrt: \n");
    print_mat(ddg_sqrt, size[0]);

    printf("\n");

    printf("lnorm_mat: \n");
    print_mat(lnorm_mat, size[0]);

    printf("\n");

    test_jacoby();*/

    return 0;
}
