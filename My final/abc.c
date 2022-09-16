/*this function get jacobi matrix*/
double **vectors_matrix(double **mat, int N, int k){/*do step 4-5 in algorithm 1*/

    double **vec_mat, **t_mat, **sorted_jac;
    int i, j;
    double tmp, sum;
    int index;
    int real_size_of_jac;/*jacobi is (Nx1)xN*/

    real_size_of_jac = N-1;

    printf("jac matrix\n");
    print_mat_normal(mat, N, N-1);
    printf("jac matrix\n");

    sorted_jac = (double **)calloc(N, sizeof(double *));

    vec_mat = (double **)calloc(real_size_of_jac, sizeof(double *));

    if(!vec_mat){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }

    t_mat = (double **)calloc(real_size_of_jac, sizeof(double *));

    if(!t_mat){/*calloc failed*/
        printf("An Error Has Occurred\n");
        exit(1);
    }


    for(i=0; i<real_size_of_jac; i++){
        vec_mat[i] = (double *)calloc(k, sizeof(double));

        if(!vec_mat[i]){/*calloc failed*/
            printf("An Error Has Occurred\n");
            exit(1);
        }

        t_mat[i] = (double *)calloc(k, sizeof(double));

        if(!t_mat[i]){/*calloc failed*/
            printf("An Error Has Occurred\n");
            exit(1);
        }

    }/*end of for*/

    sorted_jac[0] = (double *)calloc(real_size_of_jac, sizeof(double));

    for(i=0; i<real_size_of_jac; i++){
        sorted_jac[0][i] = mat[0][i];
    }

    qsort(sorted_jac[0], real_size_of_jac, sizeof(double), compare);

    for(i=0; i<real_size_of_jac; i++){
        sorted_jac[i+1] = (double *)calloc(k, sizeof(double));

        if(!sorted_jac[i+1]){/*calloc failed*/
            printf("An Error Has Occurred\n");
            exit(1);
        }
    }

    for(i = 1; i<N; i++){

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

            sorted_jac[j+1][i-1] = mat[j+1][index]; /*letting the largest vector as column j*/

        }/*end of inner for*/

        /*for(j=0; j<N; j++){

            mat[index+1][j] = 0; make the largest full of zeroes

        }end of inner for*/

        mat[0][index] = -1;

    }/*end of outer for*/

    printf("sorted jac matrix\n");
    print_mat_normal(sorted_jac, N, N-1);
    printf("sorted jac matrix\n");

    for(i = 0; i<k; i++){

        /*choose the first eigenvectors*/
        tmp = sorted_jac[0][0];
        index = 0;

        for(j=1; j<real_size_of_jac; j++){
            if(sorted_jac[0][j] > tmp){
                tmp = mat[0][j];
                index = j;
            }
        }


        /*index = calc_largest_vec(mat, N);*/

        for(j=0; j<real_size_of_jac; j++){

            vec_mat[j][i] = sorted_jac[i+1][j]; /*letting the largest vector as column j*/

        }/*end of inner for*/

        /*for(j=0; j<N; j++){

            mat[index+1][j] = 0; make the largest full of zeroes

        }end of inner for*/

        sorted_jac[0][index] = -1;

    }/*end of outer for*/

    printf("vec mat\n");
    print_mat_normal(vec_mat, real_size_of_jac, k);
    printf("vec mat\n");

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

            t_mat[i][j] = vec_mat[i][j]/sum;

        }/*end of inner for*/


    }/*end of outer for*/

    printf("t_mat\n");
    print_mat_normal(t_mat, real_size_of_jac, k);
    printf("t_mat\n");

    return t_mat;

}/*end of vectors_matrix function*/