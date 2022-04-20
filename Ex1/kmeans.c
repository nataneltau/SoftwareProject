
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


const int DEFAULT_ITER = 200;

typedef char* LINE;

struct linked_list{
    LINE    ln;
    struct linked_list *next;
    
};

//maybe add a func that will count the numbers of lines(point) in the file
//that way i will able to allocate arrays and not work with lists, can use it in
//while(line 32) and in array line 51

//this function make an array of string to an array of float, it basically makes the vektor from string representation to double one
double *makeDataDuoble(char *vektor_char, int dimension){//when using this func, the caller need to free memory at the end of the use
    char *token = strtok(vektor_char, ",");
    double *vektor_double = malloc(dimension);
    int k = 0;

    if(!vektor_double){//need to deal with it
        return NULL;
    }
    
    while(token != NULL){//convert from array of strings to array of doubles
        vektor_double[k] = atof(token);
        k++;
        token = strtok(NULL, ",");

    }//end of while

    return vektor_double;

}//end of function makeDataDuoble

double calc_dist(char *x, char *centroid, int dimension);//this function calculate (x-centroid)^2

int calc_argmin(int k, char *x, char (*arr_centroids)[k], int dimension);

void update_centroids(char *cluster, int dimension, char *str_centroid_buffer);

int calcLineLen(char *input_file);

int numbersOfLines(char *input_file, int line_len);

//let's build some functions
int euclidean_norm( int k, char (*arr_centroids)[k], char (*arr_prev_centroids)[k], int dimension, int iter_num){

    double small_than_e = 0.001;
    double tmp;

    if(iter_num == 0){//we are at the first iteration, return false
        return 0;
    }


    for(int i = 0; i<k; i++){

        tmp = calc_dist(arr_prev_centroids[i], arr_centroids[i], dimension);
        tmp = fabs(tmp);
        tmp = pow(tmp, 0.5);

        if(tmp >= small_than_e){
            return 0;
        }//end of if


    }//end of for
    
    return 1;
}//end of function euclidean_norm

int kmeans(int k, int max_iter, char *input_file, char *output_file){

    int iter_num = 0;
    int line_len = calcLineLen(input_file);
    int data_num = numbersOfLines(input_file, line_len)+1;
    char buff[line_len+1];
    char data_points[data_num][line_len+1];
    int dimension=0;
    
    FILE* fr = fopen(input_file, "r");

    int i = 0;

    while(fscanf(fr, "%s", buff) == 1 ){

        strcpy(data_points[i], buff);//maybe use strncpy ?
        i++;

    }//end of while

    for(i=0; i<line_len+1; i++){
        //count the numbers of commas, the dimension is the number
        //of commas + 1
        if(data_points[0][i] == ','){
            dimension++;
        }//end of if

    }//end of for

    fclose(fr);

    FILE *fw = fopen(output_file, "w");

    char arr_centroids[k][line_len];
    char arr_prev_centroids[k][line_len];
    char arr_clusters[k][data_num][line_len];
    int index_for_cluster_k[k];

    for(i = 0; i<k; i++){

        index_for_cluster_k[i] = 0;

    }//end of for
    
    for(i=0; i<k; i++){

        strcpy(arr_centroids[i], data_points[i]);//maybe use strncpy ?

    }//end of for


    int index_to_cluster;

    while((iter_num != max_iter) && !(euclidean_norm(k, arr_centroids, arr_prev_centroids, dimension, iter_num))){
        
        
        
        for(int m = 0; m<data_num; m++){

            memset(arr_clusters[m], 0, sizeof arr_clusters[m]);

        }//end of for

        for(int l = 0; l<k; l++){

            strcpy(arr_prev_centroids[l], arr_centroids[l]);//maybe use strncpy ?

        }//end of for
        
        for(int i=0; i<data_num; i++){

            index_to_cluster = calc_argmin(k, data_points[i], arr_centroids, dimension);

            strcpy(arr_clusters[index_to_cluster][index_for_cluster_k[index_to_cluster]], data_points[i]);

            index_for_cluster_k[index_to_cluster]++;

        }//end of for
        //after the for, in arr_cluster[i] there is all the x_j that u_i is their closest centroid

        //for j in range(0, k):
         //   lst_centroids[j] = update_centroids(lst_clusters[j], dimension)
        


        //remain to complete in this func (kmeans) this while loop and writing update_centroids func
        //and the writing to file after it



        iter_num++;
    }//end of while


    return 0;//the run complete succesfuly

}//end of kmeans function


int calcLineLen(char *input_file){
    FILE *fr = fopen(input_file, "r");
    int line_len = 0;

    while(fgetc(fr) != '\n'){
        line_len++;
    }//end of while

    return line_len;

}//end of function calcLineLen

//this function return the numbers of non-white-lines under the assumption that in each line there is not white space
int numbersOfLines(char *input_file, int line_len){
    
    int num_of_lines = 0;

    char buff[line_len+1];

    FILE *fr = fopen(input_file, "r");

    while(fscanf(fr, "%s", buff ) == 1){
        
        num_of_lines++;

    }//end of while

    return num_of_lines;

}//end of function numbersOfLines

int calc_argmin(int k, char *x, char (*arr_centroids)[k], int dimension){

    double min_centr = calc_dist(x, arr_centroids[0], dimension);
    int index_cluster = 0;
    double tmp = min_centr + 1;

    for(int i=1; i<k; i++){

        tmp = calc_dist(x, arr_centroids[i], dimension);

        if(tmp < min_centr){
            min_centr = tmp;
            index_cluster = i;
        }//end of if

    }//end of for

    return index_cluster;

}//end of function calc_argmin

double calc_dist(char *x, char *centroid, int dimension){

    double *x_double;
    double *centroid_double;
    double tmp = 0.0;
    double sum = 0.0;
    
    x_double = makeDataDuoble(x, dimension);
    centroid_double = makeDataDuoble(centroid, dimension);

    for(int i = 0; i<dimension; i++){

        tmp = x_double[i]-centroid_double[i];
        tmp = pow(tmp, 2);
        sum += tmp;

    }//end of for
    
    free(x_double);
    free(centroid_double);

    return sum;

}//end of function calc_dist

void update_centroids(char *cluster, int dimension, char *str_centroid_buffer){

   
}//end of function update_centroids

int main(int argc, char *argv[]){

    //before sending to the functions need to check validate of each argument

    /*if(argc == 4){
        kmeans(atoi(argv[1]), DEFAULT_ITER, argv[2], argv[3]);
    }
    else if(argc == 5){
        kmeans(atoi(argv[1]), atoi(argv[2]), argv[3], argv[4]);
    }
    else{
        printf("Invalid Input!");
        return 1;
    }*/


    //printf("%d", numbersOfLines("test line len.txt", 100));
   
    return 0;

}//end of main func