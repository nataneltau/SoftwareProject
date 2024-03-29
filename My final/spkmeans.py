import numpy as np
import pandas as pd
import sys
import os
import enum
import mykmeanssp as kpp



class goal(enum.Enum):
    spk = 1
    wam = 2
    ddg = 3
    lnorm = 4
    jacobi = 5


def calc_DL(row, centroids, cor_num):
    min_diff = sys.maxsize
    #print("a")
    for centroid in centroids:
        diff = 0
        #print("b")
        for j in range(cor_num):
            diff += ((row[j] - centroid[j]) ** 2)
        if diff < min_diff:
            min_diff = diff
        #print("c")
    return min_diff

def print_matrix(matrix, col):
    #kpp.print_mat_normal_capi(matrix, len(matrix), col)
    #print("finish mat print")
    #print("start the printing")

    #print(len(matrix))
    for element in matrix:
        el = element
        for item in range(0, len(el)-1):
            print("%.4f" %el[item], end = ',')
        print("%.4f" %el[len(el)-1])

def get_mat_size(input_file):

    try:
        fr = open(input_file, 'r')
    except OSError:
        print("Invalid Input!\n")
        return  
    
    try:
        inp_txt = fr.read()
        data_points = inp_txt.split("\n")
        #data_points = list(filter(lambda x: x == None, data_points))
        # #remove empty lines
        while("" in data_points):
            data_points.remove("")
        data_num = len(data_points)
        tmp = data_points[0].split(",")
        dimension = len(tmp)
    except:
        print("An Error Has Occurred\n")
        return

    if(dimension <= 0 or data_num <=0):
        print("Invalid Input!\n")
        fr.close()
        return
        
    fr.close()
    
    return data_num, dimension #data_num is the number of rows, dimension is the number of colmnus

def make_double_mat(file_name):
    return kpp.file_to_mat_capi(file_name) #return list of lists


def wam_func(file_name):
    row, col = get_mat_size(file_name)
    mat = make_double_mat(file_name)
    return kpp.wam_capi(mat, row, col)
    #return kpp.wam_capionn(file_name)

def ddg_func(file_name):
    row, col = get_mat_size(file_name)
    mat = make_double_mat(file_name)
    return kpp.ddg_capi(mat, row, col)

def lnorm_func(file_name):
    row, col = get_mat_size(file_name)
    mat = make_double_mat(file_name)
    return kpp.lnorm_capi(mat, row, col)

def jacobi_func(mat, size):
    return kpp.jacobi_capi(mat, size)

def heuristic(file_name):
    row, col = get_mat_size(file_name)
    mat = make_double_mat(file_name)
    return kpp.heuristic_capi(mat, row, col)

def vectors_matrix_func(file_name, k, row):
    #return kpp.vectors_matrix_capi(mat, len(mat), k)
    return kpp.vectors_matrix_another_try_capi(file_name, k, row)


def kmeans_plus_plus(k, max_iter, eps, mat):
    
    #mat = [[-1.0, 0.0], [0.0, -1.0], [0.0, -1.0], [-1.0, 0.0], [1.0, 0.0], [0.0, 1.0]]
    N = len(mat)
    cor_num = len(mat[0])
    lst_centroids = []
    centroids_indices = []
    nodes = pd.DataFrame(mat)
    np.random.seed(0)
    indexes = [i for i in range(N - 1)]
    index = np.random.choice(indexes)
    centroid = nodes.loc[index]
    lst_centroids.append(centroid)
    centroids_indices.append(index)

    #print("we inside kmeas++ in py, row is", N, "col is", cor_num)
    #print(nodes)
 
    #print("lst_centroids is:")
    #print(lst_centroids)

    for i in range(1, k):
        nodes['D'] = nodes.apply(lambda row: calc_DL(row, lst_centroids, cor_num), axis=1)
        sum_D = nodes['D'].sum()

        #print("1")

        nodes['P'] = nodes['D'].apply(lambda dl: dl / sum_D)

        #print("2")

        key = np.random.choice(nodes.index.values, p=nodes['P'])
        lst_centroids.append(nodes.loc[key])
        centroids_indices.append(key)

        #print("3")

    #print("we are about to print random choice")
    #print(centroids_indices)
    for item in range(0, k-1):
        print("%d" %centroids_indices[item], end = ',')
    print(centroids_indices[k-1])

    kpp_res = kpp.kmeans_double_capi(k, 300, 0, mat, N, cor_num, centroids_indices)   # result will be written to output

    if kpp_res != 0:
        print("An Error Has Occurred")
        sys.exit()

#
#
#kmeans++ remaining
#
#

if __name__ == '__main__':

    args = sys.argv  # args[0] = k, args[1] = goal, args[2] = file_name

    #row, col = get_mat_size("tmpFile.txt")

    list = [[1.32,2.554,3.55], [2.434,2.34,7.545], [7.5454,9.434,0.4343]]

            
    #need to check print(np.array(wam_func("firsti.txt")))



    try:#maybe accept also args == 3 , without k
        if len(args) != 4:#invalid
            print("Invalid Input!\n")
        else:
            file_name = args[3]
            goali = args[2]

            if goali == "spk":
                k = int(args[1])

                if k == 0:
                    k = heuristic(file_name)

                #print("we pass k in spk")
                
                #lnorm_mat = lnorm_func(file_name)
                #jacobi_mat = jacobi_func(lnorm_mat, len(lnorm_mat))
                #print("jac mat is:")
                #print_matrix(jacobi_mat, k)
                #print()
                #print("we start kmeans")
                row, col = get_mat_size(file_name)
                vec_mat = vectors_matrix_func(file_name, k, row)
                #print("we calc vec")
                #print_matrix(vec_mat, k)
                #print()
                kmeans_plus_plus(k, 300, 0.01, vec_mat)

            elif goali == "wam":
                #print("row and col\n")
                #row, col = get_mat_size(file_name)
                #print("row is: ", row, "col is: ", col)
                #print("\n")
                #print("normal mat")
                #mati = make_double_mat(file_name)
                #print_matrix(mati)
                #print("\n")
                mat = wam_func(file_name)
                print_matrix(mat, len(mat))
                #print("hi")

            elif goali == "ddg":
                mat = ddg_func(file_name)
                print_matrix(mat, len(mat))
            
            elif goali == "lnorm":
                mat = lnorm_func(file_name)
                print_matrix(mat, len(mat))

            elif goali == "jacobi":
                #print("enter jacobi")
                #print("row and col\n")
                row, col = get_mat_size(file_name)
                #print("row is: ", row, "col is: ", col)
                #print("\n")
                #print("normal mat")
                #print("convert mat")
                mati = make_double_mat(file_name)
                #print("finish conv")
                #print(len(mati))
                #row, col = get_mat_size(file_name)
                #mati = make_double_mat(file_name)
                #print("start calc")
                mat_jacobi = jacobi_func(mati, row)
                #print("finish calc")
                print_matrix(mat_jacobi, len(mat_jacobi))

            else: 
                print("Invalid Input!\n")

    except:
        pass

"""
    try:
        fr = open("tmpFile.txt", "r")
    except OSError:
        print("Invalid Input!\n")
            

    try:
        inp_txt = fr.read()
        data_points = inp_txt.split("\n")
        data_num = len(data_points)
        tmp = data_points[0].split(",")
        dimension = len(tmp)
    except:
        print("An Error Has Occurred\n")

    for i in range (0, dimension):
        data_in_float = [float(x) for x in data_points[i].split(",")]

    mat = [[],[]]
    mat[0] = [1,2,3]
    mat[1] = [4,5, 6]
    #mat[2] = [1, 1, 1]
    a = kpp.heuristic(data_in_float)
    print("mat: ")
    print(a)

    
 
    try:
        if len(args) == 3:
            if args[2] == goal.spk:
                kmeans_plus_plus(args[0], 100, 0, args[2])
            if args[2] == goal.jacobi:
                cal_jacobi(args[0], args[2])
            else:
                cal_matrix(args[0], args[1], args[2])
        else:
            print("Invalid Input!\n")
    except:
        pass
        """



"""
SOME FUNCTIONS, NOT SURE NEEDED


def calc_DL(row, centroids, cor_num):
    min_diff = sys.maxsize
    for centroid in centroids:
        diff = 0
        for j in range(cor_num):
            diff += ((row[j] - centroid['x_' + str(j)]) ** 2)
        if diff < min_diff:
            min_diff = diff
    return min_diff


def kmeans_plus_plus(k, max_iter, eps, file_name):
    with open(file_name) as f:
        content = f.read()
        cor_num = len(content.split("\n")[0].split(","))
        N = len(content.split("\n"))

    lst_centroids = []
    centroids_indices = []
    nodes = pd.read_csv(file_name, header=None, )
    np.random.seed(0)
    indexes = [i for i in range(N - 1)]
    index = np.random.choice(indexes)
    centroid = nodes.loc[index]
    lst_centroids.append(centroid)
    centroids_indices.append(index)

    for i in range(1, k):
        nodes['D'] = nodes.apply(lambda row: calc_DL(row, lst_centroids, cor_num), axis=1)
        sum_D = nodes['D'].sum()

        nodes['P'] = nodes['D'].apply(lambda dl: dl / sum_D)

        key = np.random.choice(nodes.index.values, p=nodes['P'])
        lst_centroids.append(nodes.loc[key])
        centroids_indices.append(key)

    kpp_res = kpp.fit(k, max_iter, eps, "my_nice_output.txt")   # result will be written to output

    if kpp_res != 0:
        print("An Error Has Occurred")
        sys.exit()

    with open("my_nice_output.txt") as outputf:
        outputf_read = outputf.read()
        the_lines_to_print = outputf_read.split("\n")
        for element in the_lines_to_print:
            print(element)


"""
