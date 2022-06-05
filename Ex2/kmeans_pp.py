import numpy as np
import pandas as pd
import sys




def kmeans_plus_plus(k, max_iter, eps, file_num1, file_num2):
    
    #fw1 = open(file_num1, "r")
    fop = open(file_num1, "r")
    check_for_number_of_data_points = fop.read()

    lst_centroids = []
    

    N = len(check_for_number_of_data_points.split("\n"))
    D_pscode = [0]*N

    fop.close()

    fw1 = pd.read_csv(file_num1, header=None)
    fw2 = pd.read_csv(file_num2, header=None)

    nodes = pd.merge(fw1,fw2, on='0')
    #print(nodes.head())
    
    np.random.seed(0)
    indexes = [i for i in range(N - 1)]
    index = np.random.choice(indexes)
    centroid = nodes.iloc[index]
    lst_centroids.append(centroid)
    for i in range(1, k):
        P_xl = []  
        D_l = calc_Dl(nodes, lst_centroids)#simular to line 4 in alg 1 pesudo code
        
        for l in range(0, N):#simular to line 5 in alg 1 pesudo code
            p = (D_l[l]) / (sum(D_l))
            P_xl.append(p)
        index = np.random.choice(indexes, p=P_xl)
        lst_centroids.append(nodes.iloc[index])


        

    #lst_of_data1
    #lst_of_data2


    #x = [float(i) for i in x]



# arg (nodes = N data nodes, centroids = list of centroids)
# returns Dl list
def calc_Dl(nodes , centroids):
    D_l = []
    for index, row in nodes.iterrows():
        D_l.append(argmin(row, centroids))
    return D_l

#this function recieve data point (node) and list of centroids
#return min(node - centroids[j])^2 for all: 1<=j<=len(centroids)
def argmin(node, centroids):
    diff = 0
    for i, cor in enumerate(node):
        diff += (cor - centroids[0][i]) ** 2
    min_diff = diff

    for centroid in centroids:
        diff = 0
        for i, cor in enumerate(node):
            diff += (cor - centroid[i]) ** 2
        if diff < min_diff:
            min_diff = diff
    return min_diff

        
            



if __name__ == '__main__':
    
    args = sys.argv

    if len(args) == 5:
        kmeans_plus_plus(int(args[1]), int(args[2]), args[3], args[4])
    elif len(args) == 4:
        kmeans_plus_plus(int(args[1]), 300,args[2], args[3])
    else:
        print("Invalid Input!\n")


    #kmeans_plus_plus(1,1,1,"input_1_db_1.txt",1)


indexes = [i for i in range(N)]
index = np.random.choice(indexes)
centroid = nodes.iloc[index]
lst_centroids.append(centroid)


centroid = np.random.choice(nodes.iloc[:, 0], p=P_xl)
cent = nodes.loc[nodes['0'] == centroid]
lst_centroids.append(centroid)


def tr(nodes , centroids):
    D_l = []
    df = nodes.copy()
    df = df.reset_index()
    for index, row in nodes.iterrows():
        D_l.append(argmin(row, centroids))
    return D_l
    


