/*this function get jacobi matrix*/
double **vectors_matrix(double **mat, int N, int k){/*do step 4-5 in algorithm 1*/

    double **vec_mat;
    int i, j;
    double tmp, sum;
    int index;
    int real_size_of_jac;/*jacobi is (Nx1)xN*/
    
    real_size_of_jac = N-1;

    vec_mat = (double **)calloc(real_size_of_jac, sizeof(double *));

    if(!vec_mat){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
}

    for(i=0; i<real_size_of_jac; i++){
        vec_mat[i] = (double *)calloc(k, sizeof(double));

        if(!vec_mat[i]){/*calloc failed*/
            printf("An Error Has Occurred\n");
            exit(1);
            }
    }/*end of for*/

    for(i = 0; i<k; i++){
    
        /*choose the first eigenvectors*/
        tmp = mat[0][0];
        index = 0;

        for(j=1; j<real_size_of_jac; j++){
            if(mat[0][j] > tmp){
                tmp = mat[0][j];
                index = j;
            }
        }

 
        /*index = calc_largest_vec(mat, N);*/

        for(j=0; j<real_size_of_jac; j++){

            vec_mat[j][i] = mat[j+1][index]; /*letting the largest vector as column j*/

        }/*end of inner for*/
    
        /*for(j=0; j<N; j++){

            mat[index+1][j] = 0; make the largest full of zeroes

        }end of inner for*/

        mat[0][index] = 0;

    }/*end of outer for*/
    
    print_mat_normal(vec_mat, real_size_of_jac, k);

    /*step 5 of alg, renormalizing each of vec_mat rows*/

    for(i=0; i<real_size_of_jac; i++){

        sum =0.0;

        for(j=0; j<k; j++){

            tmp = vec_mat[i][j];
            tmp = pow(tmp, 2);
            sum += tmp;


        }/*end of inner for*/

        sum = sqrt(sum);
    
        if(sum == 0){
            continue;
    }

        for(j=0; j<k; j++){

            vec_mat[i][j] /= sum;

        }/*end of inner for*/


    }/*end of outer for*/

    return vec_mat;

}/*end of vectors_matrix function*/