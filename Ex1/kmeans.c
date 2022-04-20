#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


const int DEFAULT_ITER = 200;


//maybe add a func that will count the numbers of lines(point) in the file
//that way i will able to allocate arrays and not work with lists, can use it in
//while(line 32) and in array line 51

//this function make an array of string to an array of double, it basically makes the vektor from string representation to double one and puts it in vektor_double
void makeDataDuoble(int line_len, char vektor_char[line_len], int dimension, double *vektor_double){

    char copy_vektor_char[line_len];
    double num;

    strcpy(copy_vektor_char, vektor_char);

    char *token = strtok(copy_vektor_char, ",");
    int k = 0;
    
    //if(token == NULL){
    //    printf("Nulli\n");
    //}

    //printf("token: %s\n", token);

    while(token != NULL && k<dimension){//convert from array of strings to array of doubles
        num = atof(token);
        vektor_double[k] = num;
        k++;
        token = strtok(NULL, ",");

    }//end of while

}//end of function makeDataDuoble

int ftoa(double n, char* res, int afterpoint, int index);//convert double to string

double calc_dist(char *x, char *centroid, int dimension, int line_len);//this function calculate (x-centroid)^2

int calc_argmin(int k, int line_len, char *x, char arr_centroids[k][line_len], int dimension);//return index j for S_j: argmin

int calcLineLen(char *input_file);//calculate the maximum line len

int numbersOfLines(char *input_file, int line_len);//return the numbers of lines in the file

//let's build some functions
int euclidean_norm( int k, int line_len, char arr_centroids[k][line_len], char arr_prev_centroids[k][line_len], int dimension, int iter_num){//check condition to while loop in kmeans function

    double small_than_e = 0.001;
    double tmp;

    if(iter_num == 0){//we are at the first iteration, return false
        return 0;
    }


    for(int i = 0; i<k; i++){//each iteration check if there is a centroid which his euclidean norm bigger than 0.001

        tmp = calc_dist(arr_prev_centroids[i], arr_centroids[i], dimension, line_len);
        tmp = fabs(tmp);
        tmp = pow(tmp, 0.5);

        if(tmp >= small_than_e){//if this true than there is at least one centroid which his euclidean norm bigger than 0.001
            return 0;
        }//end of if


    }//end of for
    
    return 1;
}//end of function euclidean_norm

//this func calculate the new centroids, like line6 in the alg in ex1
void update_centroids(int clus_len, int line_len, int data_num, char cluster[data_num][line_len], int dimension, char *str_centroid_buffer){

    double the_new_centroid[dimension];

    if(clus_len == 0){//cluster to this centroid is empty, never should get in this block
        return;
    }//end of if

    for(int m =0; m<dimension; m++){//initialization
        the_new_centroid[m]=0;
    }//end of for

    for(int i=0; i<clus_len; i++){//sum each coordinate for all the vektors in the cluster

        double clus_double[dimension];
        makeDataDuoble(line_len, cluster[i], dimension, clus_double);

        for(int j=0; j<dimension; j++){

            the_new_centroid[j] += clus_double[j];

        }//end of inner for

    }//end of for

    for(int n=0; n<dimension; n++){//normalization
        the_new_centroid[n] = the_new_centroid[n] / clus_len;
    }//end of for

    int index = 0;

    for(int k = 0; k<dimension-1; k++){//convert the new centroid to a string
        index = ftoa(the_new_centroid[k], str_centroid_buffer, 4, index);
        str_centroid_buffer[index] = ',';
        index++;

    }//end of for
    index = ftoa(the_new_centroid[dimension-1], str_centroid_buffer, 4, index);
    str_centroid_buffer[index] = '\0';



}//end of function update_centroids

int kmeans(int k, int max_iter, char *input_file, char *output_file){

    int iter_num = 0;
    int line_len = calcLineLen(input_file);
    int data_num = numbersOfLines(input_file, line_len)+1;
    char buff[line_len+1];
    char data_points[data_num][line_len+1];
    int dimension=0;
    
    FILE* fr = fopen(input_file, "r");

    int i = 0;

    while(fscanf(fr, "%s", buff) == 1 ){//copy each line in the input_file to data_points

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

    dimension++;

    //printf("data_ point: %s\n", data_points[0]);

    fclose(fr);

    FILE *fw = fopen(output_file, "w");

    char arr_centroids[k][line_len];
    char arr_prev_centroids[k][line_len];
    char arr_clusters[k][data_num][line_len];
    int index_for_cluster_k[k];
    
    for(i=0; i<k; i++){//initialization the centroids

        strcpy(arr_centroids[i], data_points[i]);//maybe use strncpy ?

    }//end of for


    int index_to_cluster;

    while((iter_num != max_iter) && !(euclidean_norm(k,  line_len, arr_centroids, arr_prev_centroids, dimension, iter_num))){
        
        
        
        for(int m = 0; m<k; m++){//nullify arr_clusters

            memset(arr_clusters[m], 0, sizeof(arr_clusters[m]));
            index_for_cluster_k[m] = 0;

        }//end of for

        for(int l = 0; l<k; l++){//copy each vektor in arr_centroids to arr_prev_centroids for the euclidean_norm func

            strcpy(arr_prev_centroids[l], arr_centroids[l]);//maybe use strncpy ?

        }//end of for
        
        //printf("dat: %s\n", arr_centroids[0]);
        //printf("dat: %s\n", arr_centroids[1]);
        //printf("dat: %s\n", arr_centroids[2]);
        
        for(int i=0; i<data_num; i++){//after this for, in arr_cluster[i] there is all the x_j that u_i is their closest centroid

            
            index_to_cluster = calc_argmin(k, line_len, data_points[i], arr_centroids, dimension);

            strncpy(arr_clusters[index_to_cluster][index_for_cluster_k[index_to_cluster]], data_points[i], sizeof(data_points[i]) );//maybe use strncpy ?

            //printf("this: %s\n", arr_clusters[index_to_cluster][index_for_cluster_k[index_to_cluster]]);
            index_for_cluster_k[index_to_cluster]++;

        }//end of for

        //printf("clust: %s\n", arr_clusters[0][0]);
        

        for(int j=0; j<k; j++){//this for is equal to the for in line 5 in the alg in ex1

            char str_centroid_buffer[line_len];

            //printf("this: %s\n", arr_clusters[j][0]);

            update_centroids(index_for_cluster_k[j], line_len, data_num, arr_clusters[j], dimension, str_centroid_buffer);

            strcpy(arr_centroids[j], str_centroid_buffer);//maybe use strncpy ?
        }//end of for
        //and the writing to file after it


        iter_num++;//increasing the number of iterations

    }//end of while

    printf("number of iter: %d\n", iter_num);
    for(int i=0; i<k; i++){//write to file the centroids
        
        fprintf(fw, "%s\n", arr_centroids[i]);

    }//end of for

    fclose(fw);

    return 0;//the run complete succesfuly

}//end of kmeans function


int calcLineLen(char *input_file){
    FILE *fr = fopen(input_file, "r");
    int line_len = 0;
    int max_line = -1;

    while(!(feof(fr))){//search the maximum line len 

        line_len = 0;

        while(fgetc(fr) != '\n'){

        line_len++;
        if(feof(fr)){
            break;
        }

        }//end of inner while

        if(max_line < line_len){
            max_line = line_len;
        }

        //if(feof(fr)){
        //    break;
        //}

    }//end of while

    return max_line;

}//end of function calcLineLen

//this function return the numbers of non-white-lines under the assumption that in each line there is not white space
int numbersOfLines(char *input_file, int line_len){
    
    int num_of_lines = 0;

    char buff[line_len+1];

    FILE *fr = fopen(input_file, "r");

    while(fscanf(fr, "%s", buff ) == 1){//count the numbers of lines
        
        num_of_lines++;

    }//end of while

    return num_of_lines;

}//end of function numbersOfLines

int calc_argmin(int k, int line_len, char x[], char arr_centroids[k][line_len], int dimension){

     //printf("centroid at start argmin: %s\n", arr_centroids[1]);
    double min_centr = calc_dist(x, arr_centroids[0], dimension, line_len);
    int index_cluster = 0;
    double tmp = min_centr + 1;

    for(int i=1; i<k; i++){//the for calc (x-u_i)^2 when u_i == arr_centroids[i], and at the end min_centr == argmin(x-u_i)^2 and index_cluster == i
        //printf("centroid send to calc_dist: %s\n", arr_centroids[i]);
        tmp = calc_dist(x, arr_centroids[i], dimension, line_len);

        if(tmp < min_centr){
            min_centr = tmp;
            index_cluster = i;
        }//end of if

    }//end of for

    return index_cluster;

}//end of function calc_argmin

double calc_dist(char *x, char *centroid, int dimension, int line_len){//calculate (x-centroid)^2

    double x_double[dimension];
    double centroid_double[dimension];
    double tmp = 0.0;
    double sum = 0.0;
    
    //printf("to makeDataDuoblre: %s\n", centroid);

    makeDataDuoble(line_len, x, dimension, x_double);//convert from string to double
    makeDataDuoble(line_len, centroid, dimension, centroid_double);//convert from string to double

    for(int i = 0; i<dimension; i++){//calculate (x-centroid)^2 for each coordinate and sum them all

        tmp = x_double[i]-centroid_double[i];
        tmp = pow(tmp, 2);
        sum += tmp;

    }//end of for


    return sum;

}//end of function calc_dist

int main(int argc, char *argv[]){

    //before sending to the functions need to check validate of each argument

    kmeans(3, 600, "input_1.txt", "output_11_cc_11.txt");

    /*if(argc == 4){
        printf("4 elements\n");
        return kmeans(atoi(argv[1]), DEFAULT_ITER, argv[2], argv[3]);
    }
    else if(argc == 5){
        printf("5 elements\n");
        return kmeans(atoi(argv[1]), atoi(argv[2]), argv[3], argv[4]);
    }
    else{
        printf("Invalid Input!");
        return 1;
    }*/


    //printf("%d", numbersOfLines("test line len.txt", 100));

    return 0;

}//end of main func



//if can't use need to do something simular wait email from Rami

// Reverses a string 'str' of length 'len'
void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}
  
// Converts a given integer x to string str[]. 
// d is the number of digits required in the output. 
// If d is more than the number of digits in x, 
// then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }
  
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';
  
    reverse(str, i);
    str[i] = '\0';
    return i;
}
  
// Converts a floating-point/double number to a string.
int ftoa(double n, char* res, int afterpoint, int index)
{
    // Extract integer part
    int ipart = (int)n;
  
    // Extract floating part
    double fpart = n - (double)ipart;
  
    // convert integer part to string
    int i = intToStr(ipart, res + index, 0);
  
    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.'; // add dot
  
        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter 
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);
  
        intToStr((int)fpart, res + i + 1 + index, afterpoint);

        return i + 1 + index;
    }
}