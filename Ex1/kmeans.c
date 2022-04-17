


const int DEFAULT_ITER = 200;

int kmeans(int k, int max_iter, int *input_file, int *output_file){

}//end of kmeans with max_iter

int kmeans(int k, int *input_file, int *output_file){
    return kmeans(k, DEFAULT_ITER, input_file, output_file);
}//end of kmeans with default max_iter==200