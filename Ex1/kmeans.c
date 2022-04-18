
#include <stdio.h>
#include <stdlib.h>

const int DEFAULT_ITER = 200;

typedef char* LINE;

struct linked_list{
    LINE    ln;
    struct linked_list *next;
    
};

int calcLineLen(char *input_file);

int kmeans(int k, int max_iter, char *input_file, char *output_file){

    int iter_num = 0;
    int line_len = calcLineLen(input_file);
    char buff[line_len+1];
    
    FILE* fr = fopen(input_file, "r");

    while(fscanf(fr, "%s", buff) == 1){

    }//end of while


    /*inp_txt = fr.read()
    data_points = inp_txt.split("\n")
    data_num = len(data_points)
    tmp = data_points[0].split(",")
    dimension = len(tmp)*/

    fclose(fr);


    return 0;//the run complete succesfuly

}//end of kmeans with max_iter

/*int kmeans(int k, char *input_file, char *output_file){
    return kmeans(k, DEFAULT_ITER, input_file, output_file);
}//end of kmeans with default max_iter==200*/


int calcLineLen(char *input_file){
    FILE *fr = fopen(input_file, "r");
    int line_len = 0;

    while(fgetc(fr) != '\n'){
        line_len++;
    }//end of while

    return line_len;

}//end of function calcLineLen

/*int main(){

    printf("%d", calcLineLen("test line len.txt"));

    return 0;
}*/