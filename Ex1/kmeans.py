import sys



def kmean(k: int, max_iter = 200, input_file = "", output_file = ""):
    #TODO check validated and handle error
    
    if(type(max_iter) != int):
        output_file = input_file
        input_file = max_iter
        max_iter = 200


    iter_num = 0
    fr = open(input_file, "r")
    inp_txt = fr.read()
    data_points = inp_txt.split("\n")
    data_num = len(data_points)
    tmp = data_points[0].split(",")
    dimension = len(tmp)

    fr.close()

    fw = open(output_file, "w")

    lst_centroids = []
    lst_prev_centroids =[]
    lst_clusters = [[]]
    
    for i in range(0, k):
        lst_centroids.append(data_points[i])
    

    while True:
        if iter_num == max_iter:
            break
        
        if euclidean_norm(lst_centroids, lst_prev_centroids, dimension):
            break
        

        lst_clusters = [[]]
        for l in range(0, k-1):
            lst_clusters.append([])

        lst_prev_centroids = lst_centroids.copy()

        for i in range(0, data_num-1):
            lst_clusters[calc_argmin(data_points[i], lst_centroids, dimension, k)].append(data_points[i])
        #after the for, in lst_cluster[i] there is all the x_j that u_i is their closest centroid

        for j in range(0, k):
            lst_centroids[j] = update_centroids(lst_clusters[j], dimension)
        
        iter_num += 1
    
    #print(iter_num)
    for element in lst_centroids:
        fw.write(element + "\n")
    
    fw.close()




def euclidean_norm(lst_centroids, lst_prev_centroids, dimension):
    small_than_e = 0.001

    if len(lst_prev_centroids) == 0:
        return False

    for i in range(0, len(lst_centroids) ):
        if (abs(calc_dist(lst_prev_centroids[i], lst_centroids[i], dimension)) ** (1/2)) >= small_than_e:
            return False
    
    return True


#in the end I am not using thin function, but maybe will so not delete
"""def calc_norm(vektor: str, dimension: int):

    norm_vek = vektor.split(",")
    norm_vek = [float(i) for i in norm_vek]
    norm = 0.0

    for element in norm_vek:
        norm += (element ** 2)

    norm = norm ** (1/2)

    return norm"""


#this function return the index of the closest cluster to x
def calc_argmin(x, lst_centroids, dimension, k):
    
    min_centr = calc_dist(x, lst_centroids[0], dimension)
    index_cluster = 0
    tmp = min_centr + 1

    for i in range (1, k):

        tmp = calc_dist(x, lst_centroids[i], dimension)

        if tmp < min_centr:
            min_centr = tmp
            index_cluster = i
    

    return index_cluster


#this function return the distance between 2 point squared
def calc_dist(x: str, centroid: str, dimension):
    x = x.split(",")
    #print(x)
    x = [float(i) for i in x]
    centroid = centroid.split(",")
    centroid = [float(i) for i in centroid]
    sum = 0.0
    tmp = 0

    for i in range (0, dimension ):
        tmp = (x[i]-centroid[i]) ** 2
        sum += tmp
    
    return sum


def update_centroids(cluster, dimension): #cluster is list
    clus_len = len(cluster)
    new_centroid = [0] * dimension
    str_centroid = ""

    if clus_len == 0:
        return None
    
    for i in range (0, clus_len):
        
        tmp_lst = cluster[i].split(",")

        for j in range(0, dimension ):

            new_centroid[j] += float(tmp_lst[j])


    for m in range(0, dimension ):
        new_centroid[m] = new_centroid[m] / clus_len


    for k in range (0, dimension - 1):

        str_centroid += str('%.4f'%new_centroid[k]) + ","
    
    str_centroid += str('%.4f'%new_centroid[dimension - 1])

    
    
    return str_centroid

#print(len(sys.argv))
if len(sys.argv) == 5:
    kmean(int(sys.argv[1]), int(sys.argv[2]), sys.argv[3], sys.argv[4])
elif len(sys.argv) == 4:
    kmean(int(sys.argv[1]), sys.argv[2], sys.argv[3])
else:
    print("Invalid Input!")
#kmeans(15, "input_3.txt", "output_44.txt")