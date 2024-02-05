import numpy as np
import networkx as nx
import matplotlib.pyplot as plt

def read_adjacency_matrix(file_path):
    return np.loadtxt(file_path, dtype=int)

def visualize_graph(adjacency_matrix):
    G = nx.Graph()
    n_nodes = len(adjacency_matrix)

    for i in range(n_nodes):
        for j in range(i + 1, n_nodes):
            if adjacency_matrix[i][j] == 1:
                G.add_edge(i, j)

    pos = nx.spring_layout(G)  # You can use other layout algorithms as well
    nx.draw(G, pos, with_labels=True, font_weight='bold', node_size=700, node_color='skyblue')
    plt.show()

if __name__ == "__main__":
    file_path = "adjacency_list.txt"  # Replace with the actual file path
    adjacency_matrix = read_adjacency_matrix(file_path)
    print("Adjacency Matrix:")
    print(adjacency_matrix)

    visualize_graph(adjacency_matrix)
