import pandas as pd


fw1 = pd.read_csv("input_1_db_1.txt", header=None)
fw2 = pd.read_csv("input_1_db_2.txt", header=None)

nodes = pd.merge(fw1,fw2, on='0')
print(nodes.head())