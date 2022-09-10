import numpy as np
import pandas as pd
import sys
import os
import mykmeanssp as kpp

def calc_DL(row, centroids, cor_num):
    min_diff = sys.maxsize
    for centroid in centroids:
        diff = 0
        for j in range(cor_num):
            diff += ((row[j] - centroid['x_' + str(j)]) ** 2)
        if diff < min_diff:
            min_diff = diff
    return min_diff

def kmeans_plus_plus(k, max_iter, eps, file_num1, file_num2, file_num3 = "my_nice_output.txt"):
    try:
        fop = open(file_num1, "r")
    except OSError:
        print("An Error Has Occurred", file_num1)
        sys.exit()
    
    check_for_number_of_data_points = fop.read()
    fop.close()

    lst_centroids = []

    try:
        f = open(file_num1, "r")
    except OSError:
        print("An Error Has Occurred")
        sys.exit()

    row = f.readline()
    cor_num_per_input = len(row.split(",")) - 1
    cor_num = 2 * cor_num_per_input

    f.close()


    N = len(check_for_number_of_data_points.split("\n"))

    

    fw1 = pd.read_csv(file_num1, header=None, names=['key'] + ['x_' + str(i) for i in range(cor_num_per_input)])
    fw2 = pd.read_csv(file_num2, header=None, names=["key"] + ['x_' + str(cor_num_per_input+i) for i in range(cor_num_per_input)])

    try:
        inputf = open("my_nice_input.txt", "w")
        outputf = open(file_num3, "w+")
    except OSError:
        print("An Error Has Occurred")
        sys.exit()


    nodes = pd.merge(fw1, fw2, on='key')

    nodes = nodes.set_index('key')

    np.random.seed(0)
    indexes = [i for i in range(N-1)]
    index = np.random.choice(indexes)
    centroid = nodes.loc[index]
    lst_centroids.append(centroid)
    
    nodes = nodes.sort_values(by=['key'])
    input_info = nodes.to_csv(index=False, header=False, line_terminator="\n")
    inputf.write(input_info)

    for i in range(1, k):
        nodes['D'] = nodes.apply(lambda row: calc_DL(row, lst_centroids, cor_num), axis=1)
        sum_D = nodes['D'].sum()


        nodes['P'] = nodes['D'].apply(lambda dl: dl/sum_D)

        key = np.random.choice(nodes.index.values, p=nodes['P'])
        lst_centroids.append(nodes.loc[key])
    res = []
    for centroid in lst_centroids:
        res.append(int(centroid.name))


    for i in range(len(res)-1):
        outputf.write(str(res[i]) + ",")

    outputf.write(str(res[-1]) + "\n")

    for centroid in lst_centroids:
        outputf.write(centroid[['x_' + str(i) for i in range(cor_num-1)]].to_csv(index=False, header=False, line_terminator=","))
        outputf.write(centroid[['x_' + str(cor_num-1)]].to_csv(index=False, header=False, line_terminator="\n"))
    
    outputf.close()
    inputf.close()


    kpp_res = kpp.fit(k ,max_iter, eps, "my_nice_input.txt", "my_nice_output.txt")

    if kpp_res != 0:
        print("An Error Has Occurred")
        sys.exit()

    try:
        outputf = open(file_num3, "r")
    except OSError:
        print("An Error Has Occurred")
        sys.exit()

    outputf.seek(0)


    outputf_read = outputf.read()
    the_lines_to_print = outputf_read.split("\n")
    for element in the_lines_to_print:
        print(element)

    outputf.close()


    if os.path.exists("my_nice_input.txt"):
        os.remove("my_nice_input.txt")

    for i in range(10000):
        pass

    if os.path.exists("my_nice_output.txt"):
        os.remove("my_nice_output.txt")


        
            



if __name__ == '__main__':
    
    args = sys.argv

    #kmeans_plus_plus(3, 333, 0,"input_1_db_1.txt", "input_1_db_2.txt")
    
    try:
        if len(args) == 6:#with max_iter
            kmeans_plus_plus(int(args[1]), int(args[2]), float(args[3]),args[4], args[5])
        elif len(args) == 5:#without max iter
            kmeans_plus_plus(int(args[1]), 300, float(args[2]) ,args[3], args[4])
        else:
            print("Invalid Input!\n")
    except:
        pass
        #print("Invalid Input!\n")

    #kmeans_plus_plus(1,1,1,"input_1_db_1.txt",1)

