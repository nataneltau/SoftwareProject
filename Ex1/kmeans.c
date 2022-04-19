
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

double calc_dist(char *x, char *centroid, int dimension);

int calcLineLen(char *input_file);

int numbersOfLines(char *input_file, int line_len);

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

        strncpy(data_points[i], buff, line_len+1);
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

    for(i=0; i<k; i++){

        strncpy(arr_centroids[i], data_points[i], line_len+1);

    }//end of while




    while((iter_num != max_iter)/* && !(euclidean_norm())*/){
        //remain to complete in this func (kmeans) this while loop
        //and the writing to file after it


        //https://www.delftstack.com/howto/c/clear-array-in-c/ for lst_clusters = [[]] line 42 in python code

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

double calc_dist(char *x, char *centroid, int dimension){

    char *token = strtok(x, ",");
    double x_double[dimension];
    int k = 0;
    double tmp = 0.0;
    double sum = 0.0;
    
    while(token != NULL){//convert from array of strings to array of doubles
        x_double[k] = atof(token);
        k++;
        token = strtok(NULL, ",");

    }//end of while

    token = strtok(centroid, ",");
    double centroid_double[dimension];
    k = 0;

    while(token != NULL){//convert from array of strings to array of doubles
        centroid_double[k] = atof(token);
        k++;
        token = strtok(NULL, ",");

    }//end of while

    for(int i = 0; i<dimension; i++){

        tmp = x_double[i]-centroid_double[i];
        tmp = pow(tmp, 2);
        sum += tmp;

    }//end of for
    
    return sum;

}//end of function calc_dist

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