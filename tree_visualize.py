import os
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import networkx as nx
from networkx.drawing.nx_agraph import graphviz_layout

def draw_blockchain(G, filename):
    pos = graphviz_layout(G, prog="dot")
    pos = {k: (-y, x) for k, (x, y) in pos.items()}
    fig = plt.figure(dpi=300)
    n = len(G)
    node_size = 7000 // n
    font_size = min(8, 700 // n)
    edge_width = min(1.0, font_size / 4)
    nx.draw(G, pos, 
        node_size=node_size, 
        font_size=font_size, 
        arrowsize=font_size, 
        width=edge_width, 
        node_color='darkred', 
        with_labels=True, 
        font_color="white"
    )
    plt.savefig(filename, bbox_inches='tight')
    plt.close()

def main():
    # Create a folder to save PNG files
    folder_name = 'trees'
    if not os.path.exists(folder_name):
        os.makedirs(folder_name)
    else:
        for file_name in os.listdir(folder_name):
            file_path = os.path.join(folder_name, file_name)
            os.remove(file_path)

    # Read the text file containing trees
    with open('edge_trees.txt', 'r') as file:
        trees = file.read().strip().split('\n\n')

    # Process each tree
    for i, tree in enumerate(trees):
        # Create a directed graph for the current tree
        edges = [line.strip().split('-') for line in tree.split('\n') if line.strip()]
        G = nx.DiGraph()
        G.add_edges_from(edges)

        # Draw and save the tree as a PNG file
        filename = os.path.join(folder_name, f'tree_{i}.png')
        draw_blockchain(G, filename)
        print(f"Tree {i} saved as {filename}")

if __name__ == "__main__":
    main()
